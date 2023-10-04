/**
 * 東方BGM on VGS for RasberryPi Pico
 * License under GPLv3: https://github.com/suzukiplan/tohovgs-pico/blob/master/LICENSE.txt
 */

#include "model.h"
#include "roms.hpp"
#include "vgssdk.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern VGS vgs;
static bool allSongFlag = false;
#define abs(x) (x < 0 ? -x : x)

#define VERSION_CODE "5.0"
#define ALBUM_COUNT (sizeof(rom_songlist) / sizeof(Album))
#define COLOR_BG 0b0001000101001000
#define COLOR_LIST_BG 0b0000100010100100
#define COLOR_BG_DARK 0b0000100010100100
#define COLOR_BG_LIGHT 0b0010001010010000
#define COLOR_BLACK 0x0000
#define COLOR_GRAY 0b1000010000010000
#define COLOR_GRAY_DARK 0b0100001000001000
#define COLOR_PLAYING_SONG 0b0000000001100011
#define COLOR_RED 0xF800
#define COLOR_GREEN 0x07E0
#define COLOR_PLAYING 0xFFFF
#define COLOR_WHITE 0xFFFF

static void printSmallFont(VGS::GFX* gfx, int x, int y, const char* format, ...)
{
    char buf[64];
    va_list args;
    va_start(args, format);
    vsnprintf(buf, sizeof(buf), format, args);
    va_end(args);
    for (int i = 0; buf[i] && i < 64; i++, x += 4) {
        if ('0' <= buf[i] && buf[i] <= '9') {
            gfx->image(x, y, 4, 8, &rom_small_font[(buf[i] - '0') * 32]);
        } else if ('A' <= buf[i] && buf[i] <= 'Z') {
            gfx->image(x, y, 4, 8, &rom_small_font[320 + (buf[i] - 'A') * 32]);
        } else if (' ' == buf[i]) {
            gfx->boxf(x, y, 4, 8, COLOR_BG);
        } else if ('.' == buf[i]) {
            gfx->image(x, y, 4, 8, &rom_small_font[320 + 832]);
        } else if (':' == buf[i]) {
            gfx->image(x, y, 4, 8, &rom_small_font[320 + 832 + 32]);
        }
    }
}

static void printSmallFontT(VGS::GFX* gfx, int x, int y, const char* format, ...)
{
    char buf[64];
    va_list args;
    va_start(args, format);
    vsnprintf(buf, sizeof(buf), format, args);
    va_end(args);
    for (int i = 0; buf[i] && i < 64; i++, x += 4) {
        if ('0' <= buf[i] && buf[i] <= '9') {
            gfx->image(x, y, 4, 8, &rom_small_font[(buf[i] - '0') * 32], COLOR_BG);
        } else if ('A' <= buf[i] && buf[i] <= 'Z') {
            gfx->image(x, y, 4, 8, &rom_small_font[320 + (buf[i] - 'A') * 32], COLOR_BG);
        } else if ('.' == buf[i]) {
            gfx->image(x, y, 4, 8, &rom_small_font[320 + 832], COLOR_BG);
        } else if (':' == buf[i]) {
            gfx->image(x, y, 4, 8, &rom_small_font[320 + 832 + 32], COLOR_BG);
        }
    }
}

static inline unsigned short sjis2jis(const unsigned char* sjis)
{
    unsigned char jis[2];
    unsigned short ret;
    jis[0] = sjis[0];
    jis[1] = sjis[1];
    if (jis[0] <= 0x9f) {
        jis[0] -= 0x71;
    } else {
        jis[0] -= 0xb1;
    }
    jis[0] *= 2;
    jis[0]++;
    if (jis[1] >= 0x7F) {
        jis[1] -= 0x01;
    }
    if (jis[1] >= 0x9e) {
        jis[1] -= 0x7d;
        jis[0]++;
    } else {
        jis[1] -= 0x1f;
    }
    ret = (jis[0] - 0x21) * 94;
    ret += jis[1] - 0x21;
    return ret;
}

static void printKanji(VGS::GFX* gfx, int x, int y, unsigned short color, const char* format, ...)
{
    static unsigned char bit[8] = {
        0b10000000,
        0b01000000,
        0b00100000,
        0b00010000,
        0b00001000,
        0b00000100,
        0b00000010,
        0b00000001};
    char buf[64];
    va_list args;
    va_start(args, format);
    vsnprintf(buf, sizeof(buf), format, args);
    va_end(args);
    int w = 0;
    for (int i = 0; buf[i] && i < 64; i += w / 4, x += w) {
        if (240 <= x) {
            return;
        }
        if (buf[i] & 0x80) {
            w = 8;
            if (0 < x + w) {
                int jis = sjis2jis((const unsigned char*)&buf[i]) * 12;
                for (int yy = 0; yy < 12; yy++) {
                    for (int xx = 0; xx < 8; xx++) {
                        if (rom_k8x12S_jisx0208[jis + yy] & bit[xx]) {
                            gfx->pixel(x + xx, y + yy, color);
                        }
                    }
                }
            }
        } else {
            w = 4;
            if (0 < x + w) {
                int jis = buf[i] * 12;
                for (int yy = 0; yy < 12; yy++) {
                    for (int xx = 0; xx < 4; xx++) {
                        if (rom_k8x12S_jisx0201[jis + yy] & bit[xx]) {
                            gfx->pixel(x + xx, y + yy, color);
                        }
                    }
                }
            }
        }
    }
}

class Position
{
  public:
    int x;
    int y;
    int w;
    int h;

    Position()
    {
        this->set(0, 0, 0, 0);
    }

    Position(int x, int y, int w, int h)
    {
        this->set(x, y, w, h);
    }

    void set(int x, int y, int w, int h)
    {
        this->x = x;
        this->y = y;
        this->w = w;
        this->h = h;
    }

    inline bool hitCheck(int tx, int ty)
    {
        return x <= tx && tx < x + w && y <= ty && ty < y + h;
    }
};

enum class DialogType {
    None,
    Seeking,
    MasterVolume,
};

struct Dialog {
    bool on;
    DialogType type;
    Position pos;
} dialog;

static bool showDialog(DialogType type)
{
    vgs.gfx.setViewport(dialog.pos.x, dialog.pos.y, dialog.pos.w, dialog.pos.h);
    if (!dialog.on) {
        dialog.on = true;
        dialog.type = type;
        for (int y = 0; y < dialog.pos.h; y++) {
            for (int x = 0; x < dialog.pos.w; x += 2) {
                vgs.gfx.pixel(x + (y & 1), y, 0x0000);
            }
        }
        return true;
    }
    return false;
}

static void hideDialog()
{
    dialog.on = false;
    dialog.type = DialogType::None;
}

static void showSeekingDialog(int percent)
{
    static int prevPercent;
    static const int w = 200;
    static const int h = 60;
    int x = (dialog.pos.w - w) / 2;
    int y = (dialog.pos.h - h) / 2;
    if (showDialog(DialogType::Seeking)) {
        vgs.gfx.startWrite();
        vgs.gfx.boxf(x, y, w, h, COLOR_WHITE);
        vgs.gfx.box(x, y, w, h, COLOR_BLACK);
        printKanji(&vgs.gfx, 92, y + 8, COLOR_BLACK, "NOW SEEKING...");
        for (int i = 0; i < 33; i++) {
            vgs.gfx.box(x + 17 + i * 5, y + 28, 4, 16, COLOR_BLACK);
        }
        prevPercent = -1;
        vgs.gfx.endWrite();
    }
    if (prevPercent != percent) {
        vgs.gfx.startWrite();
        prevPercent = percent;
        for (int i = 0; i < percent / 3; i++) {
            vgs.gfx.boxf(x + 17 + i * 5, y + 28, 4, 16, COLOR_BLACK);
        }
        vgs.gfx.endWrite();
    }
}

static void showMasterVolumeDialog()
{
    static bool touching;
    static bool ignoreTouch;
    static const int w = 232;
    static const int h = 60;
    int x = (dialog.pos.w - w) / 2;
    int y = (dialog.pos.h - h) / 2;
    int masterVolume = vgs.bgm.getMasterVolume();

    vgs.gfx.startWrite();
    if (showDialog(DialogType::MasterVolume)) {
        vgs.gfx.boxf(x, y, w, h, COLOR_WHITE);
        vgs.gfx.boxf(x + 2, y + 2, w - 4, h - 4, COLOR_BLACK);
        vgs.gfx.box(x, y, w, h, COLOR_BLACK);
        printKanji(&vgs.gfx, 82, y + 8, COLOR_WHITE, "Master Volume: %3d", masterVolume);
        for (int i = 0; i < masterVolume; i++) {
            vgs.gfx.boxf(i * 2 + 20, y + 28, 1, 16, COLOR_GREEN);
        }
        touching = false;
        ignoreTouch = true;
    }

    if (ignoreTouch) {
        if (!vgs.io.touch.on) {
            ignoreTouch = false;
        }
    } else if (vgs.io.touch.on) {
        if (!touching) {
            if (vgs.io.touch.y < y + dialog.pos.y || y + dialog.pos.y + h < vgs.io.touch.y) {
                hideDialog();
            } else {
                touching = true;
            }
        }
    } else {
        touching = false;
    }

    if (touching) {
        int currentVolume = (vgs.io.touch.x - 20) / 2;
        if (currentVolume < 1) {
            currentVolume = 1;
        } else if (100 < currentVolume) {
            currentVolume = 100;
        }
        if (currentVolume != masterVolume) {
            vgs.bgm.setMasterVolume(currentVolume);
            vgs.gfx.boxf(15 * 4 + 82, y + 8, 12, 12, COLOR_BLACK);
            printKanji(&vgs.gfx, 82, y + 8, COLOR_WHITE, "Master Volume: %3d", currentVolume);
            int i;
            for (i = 0; i < currentVolume; i++) {
                vgs.gfx.boxf(i * 2 + 20, y + 28, 1, 16, COLOR_GREEN);
            }
            for (; i < 100; i++) {
                vgs.gfx.boxf(i * 2 + 20, y + 28, 1, 16, COLOR_BLACK);
            }
        }
    }

    vgs.gfx.endWrite();
}

class View
{
  public:
    VGS::GFX* gfx;
    Position pos;

    void init(VGS::GFX* gfx, int x, int y, int w, int h)
    {
        this->gfx = gfx;
        pos.x = x;
        pos.y = y;
        pos.w = w;
        pos.h = h;
    }

    virtual void onTouchStart(int tx, int ty) = 0;
    virtual void onTouchMove(int tx, int ty) = 0;
    virtual void onTouchEnd(int tx, int ty) = 0;
};

class TopBoardView : public View
{
  private:
    int previousVolume;
    Position vpos;
    Position apos;

    void renderVolumeButton()
    {
        auto currentVolume = vgs.bgm.getMasterVolume();
        if (this->previousVolume != currentVolume) {
            char buf[8];
            snprintf(buf, sizeof(buf), "%d", currentVolume);
            this->gfx->image(vpos.x, vpos.y, vpos.w, vpos.h, rom_button_volume, 0);
            printSmallFontT(this->gfx, vpos.x + (32 - strlen(buf) * 4) / 2, 20, buf);
            this->previousVolume = currentVolume;
        }
    }

    void renderListModeButton()
    {
        this->gfx->image(apos.x, apos.y, apos.w, apos.h, allSongFlag ? rom_button_swipe_off : rom_button_swipe_on, 0);
    }

    void render()
    {
        this->gfx->setViewport(pos.x, pos.y, pos.w, pos.h);
        this->vpos.set(pos.w - 36, 8, 32, 24);
        this->apos.set(pos.w - 72, 8, 32, 24);
        printSmallFont(this->gfx, 4, 4, "TOUHOU BGM ON VGS  VER %s", VERSION_CODE);
        printSmallFont(this->gfx, 4, 16, "INDEX     00000  LOOP 0");
        printSmallFont(this->gfx, 4, 24, "LEFT TIME 00:00");
        this->renderVolumeButton();
        this->renderListModeButton();
    }

  public:
    TopBoardView(VGS::GFX* gfx, int y)
    {
        init(gfx, 0, y, 240, 32);
        this->previousVolume = -1;
        this->render();
    }

    void updateMasterVolume()
    {
        this->gfx->startWrite();
        this->gfx->setViewport(pos.x, pos.y, pos.w, pos.h);
        this->renderVolumeButton();
        this->gfx->endWrite();
    }

    void onTouchStart(int tx, int ty) override
    {
        if (this->vpos.hitCheck(tx, ty)) {
            showMasterVolumeDialog();
        } else if (this->apos.hitCheck(tx, ty)) {
            allSongFlag = !allSongFlag;
            this->gfx->startWrite();
            this->gfx->setViewport(pos.x, pos.y, pos.w, pos.h);
            this->renderListModeButton();
            this->gfx->endWrite();
        }
    }

    void onTouchMove(int tx, int ty) override {}
    void onTouchEnd(int tx, int ty) override {}
};

static const bool isBlackKey[12] = {false, true, false, false, true, false, true, false, false, true, false, true};
static const int keyX[12] = {0, 2, 4, 8, 10, 12, 14, 16, 20, 22, 24, 26};

class KeyboardView : public View
{
  private:
    unsigned char key;
    unsigned char tone;

    inline void renderKey(unsigned char key, bool red = false)
    {
        if (85 <= key) return;
        int mod12 = key % 12;
        if (isBlackKey[mod12]) {
            this->gfx->boxf(32 + keyX[mod12] + key / 12 * 28, 0, 3, 7, red ? COLOR_RED : COLOR_BLACK);
        } else {
            this->gfx->boxf(32 + keyX[mod12] + key / 12 * 28, 0, 3, 9, red ? COLOR_RED : COLOR_WHITE);
            if (0 < key) {
                unsigned char left = key - 1;
                mod12 = left % 12;
                if (isBlackKey[mod12]) {
                    this->gfx->boxf(32 + keyX[mod12] + left / 12 * 28, 0, 3, 7, COLOR_BLACK);
                }
            }
            if (key < 84) {
                unsigned char right = key + 1;
                mod12 = right % 12;
                if (isBlackKey[mod12]) {
                    this->gfx->boxf(32 + keyX[mod12] + right / 12 * 28, 0, 3, 7, COLOR_BLACK);
                }
            }
        }
    }

    void render(int ch)
    {
        this->gfx->setViewport(pos.x, pos.y, pos.w, pos.h);
        this->gfx->boxf(0, 0, this->pos.w, this->pos.h, COLOR_BG);
        printSmallFont(this->gfx, 0, 0, "CH%d TRI ", ch);
        for (unsigned char i = 0; i < 85; i++) {
            this->renderKey(i);
        }
    }

  public:
    KeyboardView(VGS::GFX* gfx, int ch, int x, int y)
    {
        init(gfx, x, y, 232, 10);
        this->key = 0xFF;
        this->tone = 0;
        render(ch);
    }

    void update(unsigned char tone, unsigned char key)
    {
        if (0 == key) {
            key = 0xFF;
        }
        if (key == this->key && tone == this->tone) {
            return;
        }
        this->gfx->setViewport(pos.x, pos.y, pos.w, pos.h);
        if (key != this->key) {
            // 現在の描画位置を元に戻す
            if (this->key != 0xFF) {
                this->renderKey(this->key);
            }
            // 新しい位置に描画
            this->key = key;
            if (this->key != 0xFF) {
                this->renderKey(this->key, true);
            }
        }
        if (tone != this->tone) {
            this->tone = tone;
            switch (tone) {
                case 0: printSmallFont(this->gfx, 16, 0, "TRI"); break;
                case 1: printSmallFont(this->gfx, 16, 0, "SAW"); break;
                case 2: printSmallFont(this->gfx, 16, 0, "SQ "); break;
                default: printSmallFont(this->gfx, 16, 0, "NOZ"); break;
            }
        }
    }

    void onTouchStart(int tx, int ty) override {}
    void onTouchMove(int tx, int ty) override {}
    void onTouchEnd(int tx, int ty) override {}
};

class SeekbarView : public View
{
  private:
    int max;
    int progress;
    bool movingProgress;
    bool infinity;

    void renderDuration(int sec)
    {
        printSmallFont(this->gfx, 3, (this->pos.h - 8) / 2 + 1, "%02d:%02d", sec / 60, sec % 60);
    }

    void renderProgress()
    {
        auto pi = (this->progress * 100) / max;
        pi *= 164;
        pi /= 100;
        if (160 < pi) {
            pi = 160;
        }
        this->gfx->boxf(32 + pi, 3, 4, this->pos.h - 6, COLOR_WHITE);

        Position p;
        if (pi < 160) {
            p.x = 32 + pi + 4;
            p.y = (this->pos.h - 2) / 2;
            p.w = 160 - pi;
            p.h = 1;
            this->gfx->boxf(p.x, p.y, p.w, p.h, COLOR_GRAY);
            this->gfx->boxf(p.x, p.y + 1, p.w, p.h, COLOR_GRAY_DARK);
            this->gfx->boxf(p.x, 3, p.w, (pos.h - 6) / 2 - 1, COLOR_BG);
            this->gfx->boxf(p.x, p.y + 2, p.w, (pos.h - 6) / 2 - 1, COLOR_BG);
        }
        if (0 < pi) {
            p.x = 32;
            p.y = (this->pos.h - 2) / 2;
            p.w = pi;
            p.h = 1;
            this->gfx->boxf(p.x, p.y, p.w, p.h, COLOR_GRAY);
            this->gfx->boxf(p.x, p.y + 1, p.w, p.h, COLOR_GRAY_DARK);
            this->gfx->boxf(p.x, 3, p.w, (pos.h - 6) / 2 - 1, COLOR_BG);
            this->gfx->boxf(p.x, p.y + 2, p.w, (pos.h - 6) / 2 - 1, COLOR_BG);
        }
    }

    void renderInfinity()
    {
        this->gfx->image(pos.w - 32, (pos.h - 24) / 2, 32, 24, this->infinity ? rom_button_infinity : rom_button_loop1, 0);
    }

    void updateProgress(int max, int progress)
    {
        if (max < 1) {
            max = 1;
        }
        if (progress < 0) {
            progress = 0;
        } else if (max <= progress) {
            progress = max - 1;
        }
        this->max = max;
        this->progress = progress;
        gfx->startWrite();
        gfx->setViewport(pos.x, pos.y, pos.w, pos.h);
        this->renderDuration(progress / 22050);
        this->renderProgress();
        gfx->endWrite();
    }

  public:
    void (*onSeek)(SeekbarView* view, int max, int progress);

    SeekbarView(VGS::GFX* gfx, int y)
    {
        init(gfx, 0, y, 240, 24);
        this->movingProgress = false;
        this->onSeek = nullptr;
        this->infinity = false;
        this->updateProgress(0, 0);
        this->renderInfinity();
    }

    void onTouchStart(int tx, int ty) override
    {
        this->movingProgress = false;
        if (32 <= tx && tx < 32 + 164) {
            this->movingProgress = true;
            this->updateProgress(this->max, (tx - 32) * 100 / 164 * this->max / 100);
        } else if (32 + 164 < tx) {
            this->infinity = !this->infinity;
            gfx->startWrite();
            this->renderInfinity();
            gfx->endWrite();
        }
    }

    void onTouchMove(int tx, int ty) override
    {
        if (this->movingProgress) {
            this->updateProgress(this->max, (tx - 32) * 100 / 164 * this->max / 100);
        }
    }

    void onTouchEnd(int tx, int ty) override
    {
        if (this->movingProgress) {
            this->movingProgress = false;
            if (this->onSeek) this->onSeek(this, this->max, this->progress);
        }
    }

    void update(int max, int progress)
    {
        if (!this->movingProgress) {
            this->updateProgress(max, progress);
        }
    }

    inline bool isInfinity()
    {
        return this->infinity;
    }
};

class SongListView : public View
{
  private:
    Album* albums;
    Song playingSong;
    int playingAlbumIndex;
    int playingSongIndex;
    bool pause;
    int albumCount;
    int albumPos;
    VGS::GFX* sprite;
    int scroll;
    int scrollTarget;
    int scrollBottom;
    int contentHeight;
    int tx, ty;
    int lastMoveY;
    int flingY;
    int lastMoveX;
    int flingX;
    int swipe;
    int swipeTarget;
    int scrollTotal;
    int swipeTotal;
    bool isScroll;
    bool isSwipe;
    int pageMove;
    int touchFrames;
    bool isAllSong;

    inline void transferSprite()
    {
        this->gfx->startWrite();
        this->sprite->push(0, 0);
        this->gfx->endWrite();
    }

    bool hitCheckAll(int ty, int* aiResult, int* si)
    {
        int y = this->scroll / 128;
        y += 4;
        for (int ai = 0; ai < this->albumCount; ai++) {
            for (int i = 0; i < 32; i++) {
                auto song = &this->albums[ai].songs[i];
                if (song->name[0]) {
                    if (y < -22) {
                        y += 22;
                    } else if (pos.h <= y) {
                        return false;
                    } else {
                        if (y < ty && ty < y + 20) {
                            *aiResult = ai;
                            *si = i;
                            return true;
                        }
                        y += 22;
                    }
                }
            }
        }
        return false;
    }

    bool hitCheck(Album* album, int ty, int* ai, int* si)
    {
        int y = this->scroll / 128;
        y += 8;
        y += 16;
        y += 24;
        for (int i = 0; i < 32; i++) {
            auto song = &album->songs[i];
            if (song->name[0]) {
                if (y < -22) {
                    y += 22;
                } else if (pos.h <= y) {
                    return false;
                } else {
                    if (y < ty && ty < y + 20) {
                        *ai = this->albumPos;
                        *si = i;
                        return true;
                    }
                    y += 22;
                }
            }
        }
        return false;
    }

    int renderSongs(int ai, int x, int y, int w)
    {
        for (int i = 0; i < 32; i++) {
            if (this->albums[ai].songs[i].name[0]) {
                if (y < -22) {
                    y += 22;
                    continue;
                } else if (pos.h <= y) {
                    y += 22;
                    continue;
                }
                if (this->playingSong.name[0] && this->playingAlbumIndex == ai && i == this->playingSongIndex) {
                    this->sprite->boxf(x + 6, y, w - 12, 20, COLOR_PLAYING_SONG);
                    this->sprite->image(x + 10, y + 2, 16, 16, this->pause ? rom_icon_pause : rom_icon_play, 0x0000);
                    this->sprite->box(x + 6, y, w - 12, 20, COLOR_PLAYING);
                    printKanji(this->sprite, x + 26, y + 4, COLOR_WHITE, "%s", this->albums[ai].songs[i].name);
                } else {
                    this->sprite->boxf(x + 6, y, w - 12, 20, this->albums[ai].color);
                    this->sprite->lineV(x + 6, y, 20, COLOR_GRAY);
                    this->sprite->lineH(x + 6, y + 19, w - 12, COLOR_BLACK);
                    this->sprite->lineV(x + w - 6, y, 20, COLOR_BLACK);
                    this->sprite->lineH(x + 6, y, w - 12, COLOR_GRAY);
                    printKanji(this->sprite, x + 10, y + 4, COLOR_WHITE, "%s", this->albums[ai].songs[i].name);
                }
                y += 22;
            }
        }
        return y;
    }

    int renderAllContents(int y)
    {
        y += 4;
        for (int ai = 0; ai < this->albumCount; ai++) {
            y = renderSongs(ai, 0, y, pos.w - 8);
        }
        return y + 4;
    }

    int renderContent(int ai, int x, int y)
    {
        y += 8;
        if (-12 < y) {
            printKanji(this->sprite, x + 4, y, COLOR_WHITE, "%s", this->albums[ai].name);
        }
        y += 16;
        if (-12 < y) {
            printKanji(this->sprite, x + pos.w - strlen(this->albums[ai].copyright) * 4 - 4, y, COLOR_WHITE, "%s", this->albums[ai].copyright);
        }
        y += 24;
        y = this->renderSongs(ai, x, y, pos.w);
        y += 4;
        if (y < pos.h) {
            printKanji(this->sprite, x + 4, y, COLOR_WHITE, "Composed by ZUN.");
        } else if (this->contentHeight) {
            return this->contentHeight;
        }
        y += 16;
        if (y < pos.h) {
            printKanji(this->sprite, x + 8, y, COLOR_WHITE, "This app is an alternative fiction of the Touhou Project.");
        } else if (this->contentHeight) {
            return this->contentHeight;
        }
        y += 16;
        if (y < pos.h) {
            printKanji(this->sprite, x + 140, y, COLOR_WHITE, "Arranged by Yoji Suzuki.");
        } else if (this->contentHeight) {
            return this->contentHeight;
        }
        y += 14;
        if (y < pos.h) {
            printKanji(this->sprite, x + 100, y, COLOR_WHITE, "(C)2013, Presented by SUZUKI PLAN.");
        } else if (this->contentHeight) {
            return this->contentHeight;
        }
        return y + 20;
    }

    void renderScrollBar()
    {
        this->sprite->boxf(pos.w - 8, 8, 8, pos.h - 16, COLOR_GRAY_DARK);
        int max = pos.h - 16 - 24;
        int bottom = this->contentHeight - this->pos.h;
        if (0 != bottom) {
            this->sprite->image(pos.w - 8, -this->scrollTarget / bottom * max / 128 + 8, 8, 24, rom_scroll_bar);
        }
        this->sprite->image(pos.w - 8, 0, 8, 8, rom_scroll_end);
        this->sprite->image(pos.w - 8, pos.h - 8, 8, 8, rom_scroll_end);
    }

    void render()
    {
        gfx->setViewport(pos.x, pos.y, pos.w, pos.h);
        int x = this->swipe / 128;
        int y = this->scroll / 128;
        this->sprite->clear(COLOR_LIST_BG);
        if (this->isAllSong) {
            if (this->contentHeight) {
                this->renderAllContents(y);
            } else {
                this->contentHeight = this->renderAllContents(y);
                this->scrollBottom = (pos.h - this->contentHeight) * 128;
            }
            this->renderScrollBar();
        } else {
            if (this->contentHeight) {
                this->renderContent(this->albumPos, x, y);
            } else {
                this->contentHeight = this->renderContent(this->albumPos, x, y);
                this->scrollBottom = (pos.h - this->contentHeight) * 128;
            }
            if (x < 0) {
                int pos = this->albumPos + 1;
                pos %= this->albumCount;
                this->renderContent(pos, x + 240, 0);
            } else if (0 < x) {
                int pos = this->albumPos - 1;
                if (pos < 0) pos = this->albumCount - 1;
                this->renderContent(pos, x - 240, 0);
            }
        }
        this->transferSprite();
    }

    void play(int ai, int si)
    {
        if (ai == this->playingAlbumIndex && si == this->playingSongIndex) {
            this->pause = !this->pause;
            this->onTapSong(nullptr, this->pause);
        } else {
            memcpy(&this->playingSong, &this->albums[ai].songs[si], sizeof(this->playingSong));
            this->playingAlbumIndex = ai;
            this->playingSongIndex = si;
            this->pause = false;
            this->onTapSong(&this->playingSong, this->pause);
        }
    }

  public:
    SongListView(VGS::GFX* gfx, Album* albums, int albumCount, int y, int h)
    {
        this->albums = albums;
        this->albumCount = albumCount;
        this->albumPos = 0;
        this->isAllSong = false;
        this->resetVars();
        init(gfx, 0, y, 240, h);
        this->sprite = new VGS::GFX(pos.w, pos.h);
        memset(&this->playingSong, 0, sizeof(this->playingSong));
        this->playingAlbumIndex = -1;
        this->playingSongIndex = -1;
        this->render();
    }

    void setAllSongMode(bool on)
    {
        if (on && this->isAllSong) {
            return;
        } else if (!on && !this->isAllSong) {
            return;
        }
        this->isAllSong = on;
        this->contentHeight = 0;
        this->scroll = 0;
        this->scrollTarget = 0;
    }

    void (*onTapSong)(Song* song, bool pause);

    void playNextSong()
    {
        auto album = &this->albums[this->playingAlbumIndex];
        int nextSongIndex;
        for (nextSongIndex = this->playingSongIndex + 1; nextSongIndex < 32; nextSongIndex++) {
            if (album->songs[nextSongIndex].name[0]) {
                break;
            }
        }
        if (nextSongIndex < 32) {
            // 現在のアルバムの次の曲を演奏
            this->play(this->playingAlbumIndex, nextSongIndex);
        } else {
            // 次のアルバムの先頭の曲を演奏
            int nextAlbumIndex = this->playingAlbumIndex + 1;
            nextAlbumIndex %= this->albumCount;
            for (nextSongIndex = 0; nextSongIndex < 32; nextSongIndex++) {
                if (albums[nextAlbumIndex].songs[nextSongIndex].name[0]) {
                    this->play(nextAlbumIndex, nextSongIndex);
                    break;
                }
            }
        }
    }

    void resetVars()
    {
        this->scroll = 0;
        this->scrollTarget = 0;
        this->scrollBottom = 0;
        this->lastMoveY = 0;
        this->flingY = 0;
        this->swipe = 0;
        this->swipeTarget = 0;
        this->lastMoveX = 0;
        this->flingX = 0;
        this->contentHeight = 0;
        this->scrollTotal = 0;
        this->swipeTotal = 0;
        this->isScroll = false;
        this->isSwipe = false;
        this->pageMove = 0;
    }

    void correctOverscroll()
    {
        if (0 < this->scrollTarget || this->contentHeight < pos.h) {
            this->scrollTarget = 0;
            this->flingY = 0;
        } else if (this->scrollTarget < this->scrollBottom) {
            this->scrollTarget = this->scrollBottom;
            this->flingY = 0;
        }
    }

    void correctPagePosition()
    {
        if (2560 < abs(this->swipeTarget)) {
            if (this->swipeTarget < 0) {
                this->pageMove = -1;
            } else {
                this->pageMove = 1;
            }
        } else {
            this->swipeTarget = 0;
        }
    }

    void move()
    {
        this->touchFrames++;
        if (this->pageMove) {
            this->flingX = 0;
            this->swipeTarget = 0;
            int diff = 240 * 128 * this->pageMove - this->swipe;
            diff /= 3;
            if (128 < abs(diff)) {
                this->swipe += diff;
            } else {
                this->swipe = 0;
                this->pageMove *= -1;
                this->albumPos += this->pageMove;
                if (this->albumPos < 0) {
                    this->albumPos = this->albumCount - 1;
                } else if (this->albumCount <= this->albumPos) {
                    this->albumPos = 0;
                }
                this->resetVars();
            }
        } else {
            // move Y
            if (0 != this->flingY) {
                this->scrollTarget += this->flingY;
                this->flingY /= 3;
                this->correctOverscroll();
            }
            int diff = this->scrollTarget - this->scroll;
            diff /= 3;
            if (diff) {
                this->scroll += diff;
            } else {
                this->scroll = this->scrollTarget;
            }

            // move X
            if (0 != this->flingX) {
                this->swipeTarget += this->flingX;
                this->flingX /= 3;
                this->correctPagePosition();
            }
            diff = this->swipeTarget - this->swipe;
            diff /= 3;
            if (diff) {
                this->swipe += diff;
            } else {
                this->swipe = this->swipeTarget;
            }
        }

        this->render();
    }

    void resume()
    {
        this->pause = false;
    }

    void onTouchStart(int tx, int ty) override
    {
        if (this->pageMove) return;
        this->flingX = 0;
        this->flingY = 0;
        this->lastMoveX = 0;
        this->lastMoveY = 0;
        this->swipeTarget = this->swipe;
        this->scrollTarget = this->scroll;
        this->tx = tx;
        this->ty = ty;
        this->scrollTotal = 0;
        this->swipeTotal = 0;
        this->isScroll = false;
        this->isSwipe = false;
        this->touchFrames = 0;
    }

    void onTouchMove(int tx, int ty) override
    {
        if (this->pageMove) {
            this->tx = tx;
            this->ty = ty;
            return;
        }
        if (this->isAllSong) {
            this->tx = tx;
            this->flingX = 0;
        }
        this->lastMoveX = (tx - this->tx) * 128;
        this->lastMoveY = (ty - this->ty) * 128;

        if (!this->isScroll && !this->isSwipe) {
            this->scrollTotal += this->lastMoveY;
            this->swipeTotal += this->lastMoveX;
            if (128 * 10 < abs(this->scrollTotal) || 128 * 10 < abs(this->swipeTotal)) {
                if (abs(this->scrollTotal) < abs(this->swipeTotal)) {
                    this->isSwipe = true;
                } else {
                    this->isScroll = true;
                }
            }
        }

        if (!this->isSwipe) {
            if (0 < this->scrollTarget || this->scrollTarget < this->scrollBottom) {
                this->lastMoveY /= 6;
            }
            this->scrollTarget += this->lastMoveY;
        } else {
            this->lastMoveY = 0;
            this->scrollTarget = this->scroll;
        }

        if (this->isSwipe) {
            this->swipeTarget += this->lastMoveX;
        } else {
            this->lastMoveX = 0;
            this->swipeTarget = 0;
        }

        this->tx = tx;
        this->ty = ty;
    }

    void onTouchEnd(int tx, int ty) override
    {
        if (this->pageMove) return;

        if (1 < this->touchFrames && abs(this->scrollTotal) < 512 && abs(this->swipeTotal) < 512) {
            this->flingY = 0;
            this->flingX = 0;
            int ai, si;
            if (this->isAllSong) {
                if (this->hitCheckAll(ty, &ai, &si)) {
                    this->play(ai, si);
                }
            } else {
                if (this->hitCheck(&this->albums[this->albumPos], ty, &ai, &si)) {
                    this->play(ai, si);
                }
            }
        } else {
            this->flingY = this->lastMoveY;
            this->flingX = this->lastMoveX;
        }
        this->correctOverscroll();
        this->correctPagePosition();
    }
};

static TopBoardView* topBoard;
static KeyboardView* keys[6];
static SongListView* songList;
static SeekbarView* seekbar;
static Album* albums = (Album*)rom_songlist;

void onTapSong(Song* song, bool pause)
{
    if (song) {
        vgs.led(true);
        vgs.bgm.load(&rom_bgm[song->bgmHead], song->bgmSize);
        vgs.led(false);
    } else {
        if (pause) {
            vgs.bgm.pause();
        } else {
            vgs.bgm.resume();
        }
    }
}

extern "C" void vgs_setup()
{
    vgs.gfx.startWrite();
    vgs.gfx.clear(COLOR_BG);

    // ガイドラインを描画
    vgs.gfx.line(0, 34, 240, 34, COLOR_GRAY);
    vgs.gfx.line(0, 36, 240, 36, COLOR_WHITE);
    vgs.gfx.line(0, 101, 240, 101, COLOR_WHITE);
    vgs.gfx.line(0, 103, 240, 103, COLOR_GRAY);

    // Viewを初期化
    topBoard = new TopBoardView(&vgs.gfx, 0);
    for (int i = 0; i < 6; i++) {
        keys[i] = new KeyboardView(&vgs.gfx, i, 4, 40 + i * 10);
    }
    songList = new SongListView(&vgs.gfx, albums, sizeof(rom_songlist) / sizeof(Album), 105, 190);
    memcpy(&dialog.pos, &songList->pos, sizeof(Position));
    songList->onTapSong = onTapSong;
    seekbar = new SeekbarView(&vgs.gfx, 320 - 24);
    seekbar->onSeek = [](SeekbarView* view, int max, int progress) {
        if (view == seekbar && 22050 < max) {
            vgs.gfx.endWriteSimulatorOnly();
            vgs.bgm.seekTo(progress, [](int percent) {
                showSeekingDialog(percent);
            });
            hideDialog();
            vgs.gfx.startWriteSimulatorOnly();
            songList->resume();
        }
    };
    vgs.gfx.endWrite();
    vgs.setFrameRate(20);
}

static inline void updatePlaying()
{
    for (int i = 0; i < 6; i++) {
        keys[i]->update(vgs.bgm.getTone(i), vgs.bgm.getKey(i));
    }
    if (vgs.bgm.isPlayEnd()) {
        if (seekbar->isInfinity()) {
            vgs.bgm.seekTo(0, nullptr);
        } else {
            songList->playNextSong();
        }
    } else if (!seekbar->isInfinity() && 1 <= vgs.bgm.getLoopCount()) {
        vgs.bgm.fadeout();
    }
    seekbar->update(vgs.bgm.getLengthTime(), vgs.bgm.getDurationTime());
}

extern "C" void vgs_loop()
{
    // ダイアログ処理中は通常のループ処理を実行しない
    if (dialog.on) {
        switch (dialog.type) {
            case DialogType::None:
                hideDialog();
                break;
            case DialogType::Seeking:
                showSeekingDialog(0);
                break;
            case DialogType::MasterVolume:
                showMasterVolumeDialog();
                updatePlaying();
                topBoard->updateMasterVolume();
                break;
        }
        if (dialog.on) {
            return;
        }
    }

    // タッチイベントを対象Viewへ配送
    static View* touchingView = nullptr;
    static bool prevTouched = false;
    static int prevTouchX = 0;
    static int prevTouchY = 0;
    if (prevTouched && vgs.io.touch.on && touchingView) {
        if (vgs.io.touch.x != prevTouchX || vgs.io.touch.y != prevTouchY) {
            touchingView->onTouchMove(vgs.io.touch.x - touchingView->pos.x, vgs.io.touch.y - touchingView->pos.y);
        }
    } else if (!prevTouched && vgs.io.touch.on) {
        if (seekbar->pos.y <= vgs.io.touch.y) {
            touchingView = seekbar;
        } else if (vgs.io.touch.y < topBoard->pos.h) {
            touchingView = topBoard;
        } else {
            touchingView = songList;
        }
        if (touchingView) {
            touchingView->onTouchStart(vgs.io.touch.x - touchingView->pos.x, vgs.io.touch.y - touchingView->pos.y);
        }
    } else if (!vgs.io.touch.on && prevTouched && touchingView) {
        touchingView->onTouchEnd(prevTouchX - touchingView->pos.x, prevTouchY - touchingView->pos.y);
        touchingView = nullptr;
    }
    prevTouched = vgs.io.touch.on;
    if (vgs.io.touch.on) {
        prevTouchX = vgs.io.touch.x;
        prevTouchY = vgs.io.touch.y;
    }

    // アニメーション対象Viewを再描画
    if (!dialog.on) {
        songList->setAllSongMode(allSongFlag);
        songList->move();
        updatePlaying();
    }
}
