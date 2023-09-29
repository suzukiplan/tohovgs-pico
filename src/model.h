/**
 * 東方BGM on VGS for RasberryPi Pico
 * License under GPLv3: https://github.com/suzukiplan/tohovgs-pico/blob/master/LICENSE.txt
 */

#ifndef INCLUDE_MODEL_H
#define INCLUDE_MODEL_H

typedef struct Song_ {
    int bgmSize;            // size of bgm (0 = no data)
    int bgmHead;            // &rom_bgm[bgmHead]
    int no;                 // number of song
    char loop;              // loop flag
    char reserved[3];       // padding
    char name[64];          // song name (SJIS)
} Song;

typedef struct Album_ {
    char name[64];          // album name (SJIS)
    char copyright[32];     // copyright (SJIS)
    unsigned short color;   // color code (RGB565)
    short reserved;         // padding
    Song songs[32];  // song records (max 32 songs per album)
} Album;

#endif
