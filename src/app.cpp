/**
 * 東方BGM on VGS for RasberryPi Pico
 * License under GPLv3: https://github.com/suzukiplan/tohovgs-pico/blob/master/LICENSE.txt
 */

#include <SPI.h>
#include <TFT_eSPI.h>

static TFT_eSPI gfx(240, 320);

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
    gfx.fillScreen(0x003F);
    gfx.setCursor(0, 0, 2);
    gfx.setTextColor(0xFFFF);
    gfx.setTextSize(2);
    gfx.println("Hello, World!");
    gfx.println("Using TFT_eSPI");
    gfx.endWrite();
}

void loop()
{
    delay(1000);
    digitalWrite(25, LOW);
    delay(1000);
    digitalWrite(25, HIGH);
}
