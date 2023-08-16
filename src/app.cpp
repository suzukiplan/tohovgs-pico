/**
 * 東方BGM on VGS for RasberryPi Pico
 * License under GPLv3: https://github.com/suzukiplan/tohovgs-pico/blob/master/LICENSE.txt
 */

#include "roms.hpp"
#include <SPI.h>
#include <TFT_eSPI.h>

#define COLOR_BG 0b0001000101001000
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

typedef struct Position_ {
    int x;
    int y;
    int w;
    int h;
} Position;

class TopBoard
{
  private:
    TFT_eSPI* gfx;
    Position pos;

    void render()
    {
        printSmallFont(this->gfx, this->pos.x + 4, this->pos.y + 4, "SONG NOT SELECTED");
        printSmallFont(this->gfx, this->pos.x + 4, this->pos.y + 16, "INDEX     00000  PLAYING 0 OF 0");
        printSmallFont(this->gfx, this->pos.x + 4, this->pos.y + 24, "LEFT TIME 00:00");
    }

  public:
    TopBoard(TFT_eSPI* gfx, int y)
    {
        this->gfx = gfx;
        this->pos.x = 0;
        this->pos.y = y;
        this->pos.w = 240;
        this->pos.h = 32;
        this->render();
    }
};

class Keyboard
{
  private:
    TFT_eSPI* gfx;
    Position pos;
    int key;

    void render(int ch)
    {
        this->gfx->fillRect(this->pos.x, this->pos.y, this->pos.w, this->pos.h, COLOR_BG);
        // チャネル名と楽器を描画
        printSmallFont(this->gfx, this->pos.x, this->pos.y, "CH%d TRI ", ch);
        // 白鍵を描画
        for (int i = 0; i < 50; i++) {
            this->gfx->fillRect(this->pos.x + 32 + i * 4, this->pos.y, 3, 9, COLOR_WHITE);
        }
        // 黒鍵を描画
        for (int i = 0; i < 49; i++) {
            switch (i % 7) {
                case 0:
                case 2:
                case 3:
                case 5:
                case 6:
                    this->gfx->fillRect(this->pos.x + 32 + i * 4 + 2, this->pos.y, 3, 7, COLOR_BLACK);
                    break;
            }
        }
    }

  public:
    Keyboard(TFT_eSPI* gfx, int ch, int x, int y)
    {
        this->gfx = gfx;
        this->pos.x = x;
        this->pos.y = y;
        this->pos.w = 232;
        this->pos.h = 10;
        this->key = -1;
        render(ch);
    }
};

class Seekbar
{
  private:
    TFT_eSPI* gfx;
    Position pos;

    void render()
    {
        this->gfx->drawLine(204, this->pos.y, 204, this->pos.y + this->pos.h, COLOR_GRAY);
        this->gfx->drawLine(0, this->pos.y, 240, this->pos.y, COLOR_GRAY);
        this->renderDuration(0);
        this->renderProgress(100, 0);
    }

  public:
    Seekbar(TFT_eSPI* gfx, int y)
    {
        this->gfx = gfx;
        this->pos.x = 0;
        this->pos.y = y;
        this->pos.w = 240;
        this->pos.h = 24;
        render();
    }

    void renderDuration(int sec)
    {
        printSmallFont(this->gfx, 4, this->pos.y + (this->pos.h - 8) / 2 + 1, "%02d:%02d", sec / 60, sec % 60);
    }

    void renderProgress(int max, int progress)
    {
        Position p;
        progress = (progress * 100) / max;
        progress *= 164;
        progress /= 100;
        if (160 < progress) progress = 160;
        this->gfx->fillRect(32 + progress, this->pos.y + 3, 4, this->pos.h - 6, COLOR_WHITE);
        if (progress < 160) {
            p.x = 32 + progress + 4;
            p.y = this->pos.y + (this->pos.h - 2) / 2;
            p.w = 160 - progress;
            p.h = 1;
            this->gfx->fillRect(p.x, p.y, p.w, p.h, COLOR_GRAY);
            this->gfx->fillRect(p.x, p.y + 1, p.w, p.h, COLOR_GRAY_DARK);
            this->gfx->fillRect(p.x, pos.y + 3, p.w, (pos.h - 6) / 2 - 1, COLOR_BG);
            this->gfx->fillRect(p.x, p.y + 2, p.w, (pos.h - 6) / 2 - 1, COLOR_BG);
        }
        if (0 < progress) {
            p.x = 32;
            p.y = this->pos.y + (this->pos.h - 2) / 2;
            p.w = progress;
            p.h = 1;
            this->gfx->fillRect(p.x, p.y, p.w, p.h, COLOR_GRAY);
            this->gfx->fillRect(p.x, p.y + 1, p.w, p.h, COLOR_GRAY_DARK);
            this->gfx->fillRect(p.x, pos.y + 3, p.w, (pos.h - 6) / 2 - 1, COLOR_BG);
            this->gfx->fillRect(p.x, p.y + 2, p.w, (pos.h - 6) / 2 - 1, COLOR_BG);
        }
    }
};

static TFT_eSPI gfx(240, 320);
static TopBoard* topBoard;
static Keyboard* keys[6];
static Seekbar* seekbar;

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

    // ガイドラインを描画
    gfx.drawLine(0, 34, 240, 34, COLOR_GRAY);
    gfx.drawLine(0, 36, 240, 36, COLOR_WHITE);
    gfx.drawLine(0, 101, 240, 101, COLOR_WHITE);
    gfx.drawLine(0, 103, 240, 103, COLOR_GRAY);

    // Viewを初期化
    topBoard = new TopBoard(&gfx, 0);
    for (int i = 0; i < 6; i++) {
        keys[i] = new Keyboard(&gfx, i, 4, 40 + i * 10);
    }
    seekbar = new Seekbar(&gfx, 320 - 24);

    gfx.endWrite();
}

void loop()
{
    delay(1000);
    digitalWrite(25, LOW);
    delay(1000);
    digitalWrite(25, HIGH);
}
