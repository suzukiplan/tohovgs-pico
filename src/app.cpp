/**
 * 東方BGM on VGS for RasberryPi Pico
 * License under GPLv3: https://github.com/suzukiplan/tohovgs-pico/blob/master/LICENSE.txt
 */

#include "roms.hpp"
#include <SPI.h>
#include <TFT_eSPI.h>

#define BGCOLOR 0x1148

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
            gfx->fillRect(x, y, 4, 8, BGCOLOR);
        } else if ('.' == buf[i]) {
            gfx->pushImage(x, y, 4, 8, &rom_small_font[320 + 832]);
        } else if (':' == buf[i]) {
            gfx->pushImage(x, y, 4, 8, &rom_small_font[320 + 832 + 32]);
        }
    }
}

class Keyboard
{
  private:
    TFT_eSPI* gfx;
    struct Position {
        int x;
        int y;
    } pos;
    int key;

    void render(int ch)
    {
        this->gfx->fillRect(this->pos.x, this->pos.y, 200, 10, BGCOLOR);
        // チャネル名と楽器を描画
        printSmallFont(this->gfx, this->pos.x, this->pos.y, "CH%d TRI ", ch);
        // 白鍵を描画
        for (int i = 0; i < 50; i++) {
            this->gfx->fillRect(this->pos.x + 32 + i * 4, this->pos.y, 3, 9, 0xFFFF);
        }
        // 黒鍵を描画
        for (int i = 0; i < 49; i++) {
            switch (i % 7) {
                case 0:
                case 2:
                case 3:
                case 5:
                case 6:
                    this->gfx->fillRect(this->pos.x + 32 + i * 4 + 2, this->pos.y, 3, 7, 0x0000);
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
        this->key = -1;
        render(ch);
    }
};

static TFT_eSPI gfx(240, 320);
static Keyboard* keys[6];

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
    gfx.fillScreen(BGCOLOR);

    printSmallFont(&gfx, 4, 4, "SONG NOT SELECTED");
    printSmallFont(&gfx, 4, 16, "INDEX     00000  PLAYING 0 OF 0");
    printSmallFont(&gfx, 4, 24, "LEFT TIME 00:00");

    gfx.drawLine(0, 34, 240, 34, 0x8410);
    gfx.drawLine(0, 36, 240, 36, 0xFFFF);
    for (int i = 0; i < 6; i++) {
        keys[i] = new Keyboard(&gfx, i, 4, 40 + i * 10);
    }
    gfx.drawLine(0, 101, 240, 101, 0xFFFF);
    gfx.drawLine(0, 103, 240, 103, 0x8410);
    gfx.endWrite();
}

void loop()
{
    delay(1000);
    digitalWrite(25, LOW);
    delay(1000);
    digitalWrite(25, HIGH);
}
