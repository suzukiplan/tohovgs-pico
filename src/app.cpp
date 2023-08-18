/**
 * 東方BGM on VGS for RasberryPi Pico
 * License under GPLv3: https://github.com/suzukiplan/tohovgs-pico/blob/master/LICENSE.txt
 */

#include "model.h"
#include "roms.hpp"
#include <SPI.h>
#include <TFT_eSPI.h>

#define ALBUM_COUNT (sizeof(rom_songlist) / sizeof(Album))
#define COLOR_BG 0b0001000101001000
#define COLOR_LIST_BG 0b0000100010100100
#define COLOR_BG_DARK 0b0000100010100100
#define COLOR_BG_LIGHT 0b0010001010010000
#define COLOR_BLACK 0x0000
#define COLOR_GRAY 0b1000010000010000
#define COLOR_GRAY_DARK 0b0100001000001000
#define COLOR_WHITE 0xFFFF

static void printSmallFont(TFT_eSPI* gfx, int x, int y, const char* format, ...)
{
    char buf[64];
    va_list args;
    va_start(args, format);
    vsnprintf(buf, sizeof(buf), format, args);
    va_end(args);
    for (int i = 0; buf[i] && i < 64; i++, x += 4) {
        if ('0' <= buf[i] && buf[i] <= '9') {
            gfx->pushImage(x, y, 4, 8, &rom_small_font[(buf[i] - '0') * 32]);
        } else if ('A' <= buf[i] && buf[i] <= 'Z') {
            gfx->pushImage(x, y, 4, 8, &rom_small_font[320 + (buf[i] - 'A') * 32]);
        } else if (' ' == buf[i]) {
            gfx->fillRect(x, y, 4, 8, COLOR_BG);
        } else if ('.' == buf[i]) {
            gfx->pushImage(x, y, 4, 8, &rom_small_font[320 + 832]);
        } else if (':' == buf[i]) {
            gfx->pushImage(x, y, 4, 8, &rom_small_font[320 + 832 + 32]);
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

static void printKanji(TFT_eSPI* gfx, int x, int y, const char* format, ...)
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
        if (buf[i] & 0x80) {
            w = 8;
            int jis = sjis2jis((const unsigned char*)&buf[i]) * 12;
            for (int yy = 0; yy < 12; yy++) {
                for (int xx = 0; xx < 8; xx++) {
                    if (rom_k8x12S_jisx0208[jis + yy] & bit[xx]) {
                        gfx->drawPixel(x + xx, y + yy, COLOR_WHITE);
                    }
                }
            }
        } else {
            w = 4;
            int jis = buf[i] * 12;
            for (int yy = 0; yy < 12; yy++) {
                for (int xx = 0; xx < 4; xx++) {
                    if (rom_k8x12S_jisx0201[jis + yy] & bit[xx]) {
                        gfx->drawPixel(x + xx, y + yy, COLOR_WHITE);
                    }
                }
            }
        }
    }
}

typedef struct Position_ {
    int x;
    int y;
    int w;
    int h;
} Position;

class View
{
  public:
    TFT_eSPI* gfx;
    Position pos;

    void init(TFT_eSPI* gfx, int x, int y, int w, int h)
    {
        this->gfx = gfx;
        pos.x = x;
        pos.y = y;
        pos.w = w;
        pos.h = h;
    }

    virtual void onTouchStart(int tx, int ty);
    virtual void onTouchMove(int tx, int ty);
    virtual void onTouchEnd(int tx, int ty);
};

class TopBoardView : public View
{
  private:
    void render()
    {
        gfx->setViewport(pos.x, pos.y, pos.w, pos.h);
        printSmallFont(this->gfx, 4, 4, "SONG NOT SELECTED");
        printSmallFont(this->gfx, 4, 16, "INDEX     00000  PLAYING 0 OF 0");
        printSmallFont(this->gfx, 4, 24, "LEFT TIME 00:00");
    }

  public:
    TopBoardView(TFT_eSPI* gfx, int y)
    {
        init(gfx, 0, y, 240, 32);
        this->render();
    }

    void onTouchStart(int tx, int ty) override {}
    void onTouchMove(int tx, int ty) override {}
    void onTouchEnd(int tx, int ty) override {}
};

class KeyboardView : public View
{
  private:
    int key;

    void render(int ch)
    {
        gfx->setViewport(pos.x, pos.y, pos.w, pos.h);
        this->gfx->fillRect(0, 0, this->pos.w, this->pos.h, COLOR_BG);
        // チャネル名と楽器を描画
        printSmallFont(this->gfx, 0, 0, "CH%d TRI ", ch);
        // 白鍵を描画
        for (int i = 0; i < 50; i++) {
            this->gfx->fillRect(32 + i * 4, 0, 3, 9, COLOR_WHITE);
        }
        // 黒鍵を描画
        for (int i = 0; i < 49; i++) {
            switch (i % 7) {
                case 0:
                case 2:
                case 3:
                case 5:
                case 6:
                    this->gfx->fillRect(32 + i * 4 + 2, 0, 3, 7, COLOR_BLACK);
                    break;
            }
        }
    }

  public:
    KeyboardView(TFT_eSPI* gfx, int ch, int x, int y)
    {
        init(gfx, x, y, 232, 10);
        this->key = -1;
        render(ch);
    }

    void onTouchStart(int tx, int ty) override {}
    void onTouchMove(int tx, int ty) override {}
    void onTouchEnd(int tx, int ty) override {}
};

class SeekbarView : public View
{
  private:
    bool movingProgress;
    void render()
    {
        gfx->setViewport(pos.x, pos.y, pos.w, pos.h);
        this->gfx->drawLine(204, 0, 204, this->pos.h, COLOR_GRAY);
        this->gfx->drawLine(0, 0, 240, 0, COLOR_GRAY);
        this->renderDuration(0);
        this->renderProgress(100, 0);
    }

    void renderDuration(int sec)
    {
        printSmallFont(this->gfx, 4, (this->pos.h - 8) / 2 + 1, "%02d:%02d", sec / 60, sec % 60);
    }

    void renderProgress(int max, int progress)
    {
        Position p;
        progress = (progress * 100) / max;
        progress *= 164;
        progress /= 100;
        if (160 < progress) progress = 160;
        this->gfx->fillRect(32 + progress, 3, 4, this->pos.h - 6, COLOR_WHITE);
        if (progress < 160) {
            p.x = 32 + progress + 4;
            p.y = (this->pos.h - 2) / 2;
            p.w = 160 - progress;
            p.h = 1;
            this->gfx->fillRect(p.x, p.y, p.w, p.h, COLOR_GRAY);
            this->gfx->fillRect(p.x, p.y + 1, p.w, p.h, COLOR_GRAY_DARK);
            this->gfx->fillRect(p.x, 3, p.w, (pos.h - 6) / 2 - 1, COLOR_BG);
            this->gfx->fillRect(p.x, p.y + 2, p.w, (pos.h - 6) / 2 - 1, COLOR_BG);
        }
        if (0 < progress) {
            p.x = 32;
            p.y = (this->pos.h - 2) / 2;
            p.w = progress;
            p.h = 1;
            this->gfx->fillRect(p.x, p.y, p.w, p.h, COLOR_GRAY);
            this->gfx->fillRect(p.x, p.y + 1, p.w, p.h, COLOR_GRAY_DARK);
            this->gfx->fillRect(p.x, 3, p.w, (pos.h - 6) / 2 - 1, COLOR_BG);
            this->gfx->fillRect(p.x, p.y + 2, p.w, (pos.h - 6) / 2 - 1, COLOR_BG);
        }
    }

    void updateProgress(int max, int progress)
    {
        if (progress < 0) {
            progress = 0;
        } else if (max <= progress) {
            progress = max - 1;
        }
        gfx->startWrite();
        gfx->setViewport(pos.x, pos.y, pos.w, pos.h);
        this->renderProgress(max, progress);
        gfx->endWrite();
    }

  public:
    SeekbarView(TFT_eSPI* gfx, int y)
    {
        init(gfx, 0, y, 240, 24);
        this->movingProgress = false;
        render();
    }

    void onTouchStart(int tx, int ty) override
    {
        if (32 <= tx && tx < 32 + 164) {
            this->movingProgress = true;
            this->updateProgress(164, tx - 32);
        }
    }

    void onTouchMove(int tx, int ty) override
    {
        if (this->movingProgress) {
            this->updateProgress(164, tx - 32);
        }
    }

    void onTouchEnd(int tx, int ty) override
    {
        this->movingProgress = false;
    }
};

class SongListView : public View
{
  private:
    Album* album;
    TFT_eSprite* sprite;
    int scroll;

    void render()
    {
        gfx->setViewport(pos.x, pos.y, pos.w, pos.h);
        this->sprite->fillScreen(COLOR_LIST_BG);
        int y = this->scroll;
        y += 8;
        if (-12 < y) {
            printKanji(this->sprite, 4, y, "%s", album->name);
        }
        y += 16;
        if (-12 < y) {
            printKanji(this->sprite, pos.w - strlen(album->copyright) * 4 - 4, y, "%s", album->copyright);
        }
        y += 24;
        for (int i = 0; i < 32; i++) {
            if (y < -22) {
                y += 22;
                continue;
            } else if (pos.h <= y) {
                break;
            }
            this->sprite->fillRect(6, y, pos.w - 12, 20, album->color);
            this->sprite->drawFastVLine(6, y, 20, COLOR_GRAY);
            this->sprite->drawFastHLine(6, y + 19, pos.w - 12, COLOR_BLACK);
            this->sprite->drawFastVLine(pos.w - 6, y, 20, COLOR_BLACK);
            this->sprite->drawFastHLine(6, y, pos.w - 12, COLOR_GRAY);
            printKanji(this->sprite, 10, y + 4, "%s", album->songs[i].name);
            y += 22;
        }
        this->sprite->pushSprite(0, 0);
    }

  public:
    SongListView(TFT_eSPI* gfx, Album* album, int y, int h)
    {
        this->album = album;
        this->scroll = 0;
        init(gfx, 0, y, 240, h);
        this->sprite = new TFT_eSprite(gfx);
        this->sprite->createSprite(pos.w, pos.h);
        this->sprite->setColorDepth(16);
        this->render();
    }

    void onTouchStart(int tx, int ty) override {}
    void onTouchMove(int tx, int ty) override {}
    void onTouchEnd(int tx, int ty) override {}
};

static TFT_eSPI gfx;
static TopBoardView* topBoard;
static KeyboardView* keys[6];
static SongListView* songList;
static SeekbarView* seekbar;
static Album* albums = (Album*)rom_songlist;

void setup()
{
    // 初期化中は本体LEDを点灯
    pinMode(25, OUTPUT);
    digitalWrite(25, HIGH);

    pinMode(TFT_BL, OUTPUT);
    digitalWrite(TFT_BL, HIGH);

    // ディスプレイを初期化
    gfx.init();
    gfx.startWrite();
    gfx.setRotation(2);
    gfx.fillScreen(COLOR_BG);

    // Touch Calibration を初期化
    uint16_t touch[] = {
        300,
        3600,
        300,
        3600,
        0b010};
    gfx.setTouch(touch);

    // ガイドラインを描画
    gfx.drawLine(0, 34, 240, 34, COLOR_GRAY);
    gfx.drawLine(0, 36, 240, 36, COLOR_WHITE);
    gfx.drawLine(0, 101, 240, 101, COLOR_WHITE);
    gfx.drawLine(0, 103, 240, 103, COLOR_GRAY);

    // Viewを初期化
    topBoard = new TopBoardView(&gfx, 0);
    for (int i = 0; i < 6; i++) {
        keys[i] = new KeyboardView(&gfx, i, 4, 40 + i * 10);
    }
    songList = new SongListView(&gfx, &albums[0], 105, 190);
    seekbar = new SeekbarView(&gfx, 320 - 24);

    gfx.endWrite();
    delay(500);
    digitalWrite(25, LOW);
}

void loop()
{
    // タッチイベントを対象Viewへ配送
    static View* touchingView = nullptr;
    static bool prevTouched = false;
    static uint16_t prevTouchX = 0;
    static uint16_t prevTouchY = 0;
    uint16_t touchX;
    uint16_t touchY;
    bool touched = gfx.getTouch(&touchX, &touchY, 20);
    if (prevTouched && touched && touchingView) {
        if (touchX != prevTouchX || touchY != prevTouchY) {
            touchingView->onTouchMove(touchX - touchingView->pos.x, touchY - touchingView->pos.y);
        }
    } else if (!prevTouched && touched) {
        if (seekbar->pos.y <= touchY) {
            touchingView = seekbar;
        } else {
            touchingView = nullptr;
        }
        if (touchingView) {
            touchingView->onTouchStart(touchX - touchingView->pos.x, touchY - touchingView->pos.y);
        }
    } else if (!touched && prevTouched && touchingView) {
        touchingView->onTouchEnd(prevTouchX - touchingView->pos.x, prevTouchY - touchingView->pos.y);
        touchingView = nullptr;
    }
    prevTouched = touched;
    if (touched) {
        prevTouchX = touchX;
        prevTouchY = touchY;
    }
    delay(20);

#if 0
    // タッチ座標を表示（あとで消す）
    gfx.setViewport(0, 0, 240, 320);
    gfx.startWrite();
    printSmallFont(&gfx, 8, 118, "SCREEN W:%03d H:%03d", gfx.width(), gfx.height());
    if (touched || prevTouched) {
        printSmallFont(&gfx, 8, 128, "TOUCH X:%03d Y:%03d", touchX, touchY);
    } else {
        printSmallFont(&gfx, 8, 128, "NOT TOUCHING      ");
    }
    gfx.endWrite();
#endif
}
