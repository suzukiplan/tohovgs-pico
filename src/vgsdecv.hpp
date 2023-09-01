/**
 * VGS BGM Decoder for Variable NOTEs (vgsftv compressed data)
 * License under GPLv3: https://github.com/suzukiplan/tohovgs-pico/blob/master/LICENSE.txt
 * (C)2023, SUZUKI PLAN
 */

#include "lz4.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NTYPE_ENV1 0x10
#define NTYPE_ENV2 0x20
#define NTYPE_VOL 0x30
#define NTYPE_MVOL 0x40
#define NTYPE_KEYON 0x50
#define NTYPE_KEYOFF 0x60
#define NTYPE_WAIT 0x70
#define NTYPE_PDOWN 0x80
#define NTYPE_JUMP 0x90
#define NTYPE_LABEL 0xA0
#define NTYPE_WAIT_8 0xB0
#define NTYPE_WAIT_16 0xC0
#define NTYPE_WAIT_32 0xD0

extern "C" {
extern const short* TONE1[85];
extern const short* TONE2[85];
extern const short* TONE3[85];
extern const short* TONE4[85];
};

class VGSDecoder
{
  private:
    struct BGM {
        unsigned char data[128 * 1024];
        int size;
    } bgm;

    struct Channel {
        const short* toneS;
        int* tone;
        int toneA;
        int toneR;
        unsigned char vol;
        unsigned char keyOn;
        unsigned char mute;
        unsigned int cur;
        unsigned int count;
        unsigned int env1;
        unsigned int env2;
        unsigned char toneT;
        unsigned char toneK;
        unsigned int pdown;
        unsigned int pcnt;
        int volumeRate;
    };

    struct Context {
        int nidx;
        bool play;
        bool stopped;
        unsigned short mvol;
        unsigned int waitTime;
        unsigned int hz;
        int wav[6];
        unsigned int fade;
        unsigned int fcnt;
        struct Channel ch[6];
        int mute;
        int loop;
        int fade2;
        unsigned int timeI;
        unsigned int timeL;
        unsigned int timeP;
        int loopIdx;
        int idxnum;
        int volumeRate;
    } ctx;

    inline void set_note(unsigned char cn, unsigned char t, unsigned char n)
    {
        ctx.ch[cn].tone = NULL;
        switch (t) {
            case 0: ctx.ch[cn].toneS = TONE1[n % 85]; break;
            case 1: ctx.ch[cn].toneS = TONE2[n % 85]; break;
            case 2: ctx.ch[cn].toneS = TONE3[n % 85]; break;
            default: ctx.ch[cn].toneS = TONE4[n % 85]; break;
        }
    }

    inline int get_next_note()
    {
        while (ctx.nidx < bgm.size) {
            unsigned char type = bgm.data[ctx.nidx++];
            switch (type & 0xF0) {
                case NTYPE_ENV1: {
                    unsigned short v16;
                    memcpy(&v16, &bgm.data[ctx.nidx], 2);
                    ctx.nidx += 2;
                    ctx.ch[type & 0x0F].env1 = v16;
                    break;
                }
                case NTYPE_ENV2: {
                    unsigned short v16;
                    memcpy(&v16, &bgm.data[ctx.nidx], 2);
                    ctx.nidx += 2;
                    ctx.ch[type & 0x0F].env2 = v16;
                    break;
                }
                case NTYPE_PDOWN: {
                    unsigned short v16;
                    memcpy(&v16, &bgm.data[ctx.nidx], 2);
                    ctx.nidx += 2;
                    type &= 0x0F;
                    ctx.ch[type].pdown = v16;
                    ctx.ch[type].pcnt = 0;
                    break;
                }
                case NTYPE_VOL: {
                    ctx.ch[type & 0x0F].vol = bgm.data[ctx.nidx++];
                    break;
                }
                case NTYPE_MVOL:
                    ctx.mvol = bgm.data[ctx.nidx++];
                    break;
                case NTYPE_KEYON: {
                    type &= 0x0F;
                    ctx.ch[type].keyOn = 1;
                    ctx.ch[type].count = 0;
                    ctx.ch[type].cur = 0;
                    ctx.ch[type].toneT = bgm.data[ctx.nidx++];
                    ctx.ch[type].toneK = bgm.data[ctx.nidx++];
                    set_note(type, ctx.ch[type].toneT, ctx.ch[type].toneK);
                    break;
                }
                case NTYPE_KEYOFF: {
                    type &= 0x0F;
                    ctx.ch[type].keyOn = 0;
                    ctx.ch[type].count = 0;
                    break;
                }
                case NTYPE_JUMP: {
                    int v32;
                    memcpy(&v32, &bgm.data[ctx.nidx], 4);
                    ctx.nidx = v32;
                    ctx.loop++;
                    ctx.timeP = ctx.timeI;
                    break;
                }
                case NTYPE_LABEL: {
                    break;
                }
                case NTYPE_WAIT_8: {
                    unsigned char val = bgm.data[ctx.nidx];
                    ctx.nidx++;
                    ctx.timeP += val;
                    return val;
                }
                case NTYPE_WAIT_16: {
                    unsigned short val;
                    memcpy(&val, &bgm.data[ctx.nidx], 2);
                    ctx.nidx += 2;
                    ctx.timeP += val;
                    return val;
                }
                case NTYPE_WAIT_32: {
                    int val;
                    memcpy(&val, &bgm.data[ctx.nidx], 4);
                    ctx.nidx += 4;
                    ctx.timeP += val;
                    return val;
                }
                default:
                    return 0;
            }
        }
        return 0;
    }

  public:
    bool load(const void* data, size_t size)
    {
        memset(&this->ctx, 0, sizeof(this->ctx));
        this->ctx.volumeRate = 100;
        for (int i = 0; i < 6; i++) {
            this->ctx.ch[i].volumeRate = 100;
        }
        this->bgm.size = LZ4_decompress_safe((const char*)data,
                                             (char*)this->bgm.data,
                                             (int)size,
                                             (int)sizeof(this->bgm.data));
        if (0 < this->bgm.size) {
            this->ctx.play = true;
            return true;
        } else {
            return false;
        }
    }

    void execute(void* buffer, size_t size, bool stereo = false)
    {
        if (!buffer) return;

        char* buf = (char*)buffer;
        int pw;
        int wav;
        short* bp;

        memset(ctx.wav, 0, sizeof(ctx.wav));
        memset(buf, 0, size);
        if (!ctx.play || bgm.size < 1) {
            ctx.stopped = true;
            return;
        }
        if (100 <= ctx.fade2) {
            ctx.play = false;
            return;
        }
        ctx.waitTime = get_next_note();
        if (0 == ctx.waitTime) {
            ctx.play = false;
            return;
        }
        if (0 == ctx.mvol) {
            return;
        }
        for (int i = 0; i < (int)size; i += stereo ? 4 : 2, ctx.hz++) {
            for (int j = 0; j < 6; j++) {
                if (ctx.ch[j].tone || ctx.ch[j].toneS) {
                    ctx.ch[j].cur %= ctx.ch[j].toneS[0];
                    wav = ctx.ch[j].toneS[1 + ctx.ch[j].cur];
                    ctx.ch[j].cur += 2;
                    wav *= (ctx.ch[j].vol * ctx.mvol);
                    if (ctx.ch[j].keyOn) {
                        if (ctx.ch[j].count < ctx.ch[j].env1) {
                            ctx.ch[j].count++;
                            pw = (ctx.ch[j].count * 100) / ctx.ch[j].env1;
                        } else {
                            pw = 100;
                        }
                    } else {
                        if (ctx.ch[j].count < ctx.ch[j].env2) {
                            ctx.ch[j].count++;
                            pw = 100 - (ctx.ch[j].count * 100) / ctx.ch[j].env2;
                        } else {
                            pw = 0;
                        }
                    }
                    if (!ctx.ch[j].mute) {
                        bp = (short*)(&buf[i]);
                        wav = (wav * pw / 100);
                        if (ctx.ch[j].volumeRate != 100) {
                            wav *= ctx.ch[j].volumeRate;
                            wav /= 100;
                        }
                        wav += *bp;
                        if (32767 < wav) {
                            wav = 32767;
                        } else if (wav < -32768) {
                            wav = -32768;
                        }
                        if (ctx.fade2) {
                            wav *= 100 - ctx.fade2;
                            wav /= 100;
                        }
                        bp[0] = (short)wav;
                        if (stereo) {
                            bp[1] = bp[0];
                        }
                        if (i) {
                            ctx.wav[j] += pw < 0 ? -pw : pw;
                            ctx.wav[j] >>= 1;
                        } else {
                            ctx.wav[j] = pw < 0 ? -pw : pw;
                        }
                    }
                    if (ctx.ch[j].pdown) {
                        ctx.ch[j].pcnt++;
                        if (ctx.ch[j].pdown < ctx.ch[j].pcnt) {
                            ctx.ch[j].pcnt = 0;
                            if (ctx.ch[j].toneK) {
                                ctx.ch[j].toneK--;
                            }
                            set_note(j & 0xff, ctx.ch[j].toneT, ctx.ch[j].toneK);
                        }
                    }
                }
            }
            if (ctx.volumeRate != 100) {
                bp = (short*)(&buf[i]);
                wav = (*bp) * ctx.volumeRate;
                wav /= 100;
                if (32767 < wav)
                    wav = 32767;
                else if (wav < -32768)
                    wav = -32768;
                printf("bp=%d\n", wav);
                bp[0] = (short)wav;
                if (stereo) {
                    bp[1] = bp[0];
                }
            }
            ctx.waitTime--;
            if (0 == ctx.waitTime) {
                ctx.waitTime = get_next_note();
                if (0 == ctx.waitTime) {
                    return; /* no data */
                }
            }
            /* fade out */
            if (ctx.fade2 && ctx.fade2 < 100) {
                ctx.fcnt++;
                if (1023 < ctx.fcnt) {
                    ctx.fcnt = 0;
                    ctx.fade2++;
                }
            }
            if (ctx.fade) {
                ctx.fcnt++;
                if (ctx.fade < ctx.fcnt) {
                    ctx.mvol--;
                    ctx.fcnt = 0;
                }
            }
        }
    }
};
