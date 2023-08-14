/**
 * 東方BGM on VGS for RasberryPi Pico
 * -----------------------------------------------------------------------------
 * The MIT License (MIT)
 *
 * Copyright (c) 2023 Yoji Suzuki.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 * -----------------------------------------------------------------------------
 */

#include <LovyanGFX.hpp>

class ILI9341 : public lgfx::LGFX_Device
{
  private:
    const int pinLed = 27;
    const int pinCs = 17;
    const int pinReset = 22;
    const int pinDc = 28;
    const int pinMosi = 19;
    const int pinSck = 18;
    const int pinMiso = -1;
    const int pinBusy = -1;

    lgfx::Panel_ILI9341 panel;
    lgfx::Bus_SPI bus;

    void initBusConfig()
    {
        auto cfg = this->bus.config();
        cfg.spi_host = 0;
        cfg.spi_mode = 0;
        cfg.freq_write = 40000000;
        cfg.freq_read = 16000000;
        cfg.pin_sclk = this->pinSck;
        cfg.pin_mosi = this->pinMosi;
        cfg.pin_miso = this->pinMiso;
        cfg.pin_dc = this->pinDc;
        this->bus.config(cfg);
    }

    void initPanelConfig()
    {
        auto cfg = this->panel.config();
        cfg.pin_cs = this->pinCs;
        cfg.pin_rst = this->pinReset;
        cfg.pin_busy = -1;
        cfg.panel_width = 240;
        cfg.panel_height = 320;
        cfg.offset_x = 0;
        cfg.offset_y = 0;
        cfg.offset_rotation = 2;
        cfg.dummy_read_pixel = 8;
        cfg.dummy_read_bits = 1;
        cfg.readable = false;
        cfg.invert = false;
        cfg.rgb_order = false;
        cfg.dlen_16bit = false;
        cfg.bus_shared = true;
        this->panel.config(cfg);
    }

  public:
    ILI9341(void)
    {
        this->initBusConfig();
        this->initPanelConfig();
        this->panel.setBus(&this->bus);
        setPanel(&this->panel);
        pinMode(this->pinLed, OUTPUT);
    }

    void led(bool on)
    {
        digitalWrite(this->pinLed, on ? HIGH : LOW);
    }
};

static ILI9341 gfx;

void setup()
{
    // 初期化中は本体LEDを点灯
    pinMode(25, OUTPUT);
    digitalWrite(25, HIGH);

    // ディスプレイを初期化
    gfx.init();
    gfx.startWrite();
    gfx.led(true);
    gfx.setRotation(2);
    gfx.writeFillRect(0, 0, gfx.width(), gfx.height(), 0x003F);
    gfx.setTextSize(2);
    gfx.setTextColor(0xFFFF, 0x0000);
    gfx.setCursor(0, 0);
    gfx.println("Hello, World!");
    gfx.println("Using LovyanGFX.");
    gfx.endWrite();
}

void loop()
{
    delay(1000);
    digitalWrite(25, LOW);
    delay(1000);
    digitalWrite(25, HIGH);
}
