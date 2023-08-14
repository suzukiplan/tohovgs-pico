# 東方BGM on VGS for RaspberryPi Pico

## WIP Status

まだ、ほぼほぼ何もできてません

## About

- 本リポジトリでは、同人ハードウェア版 東方BGM on VGS のファームウェアと組み立て手順を提供します
- 同人ハードウェア版 東方BGM on VGS は基本的に、各自で部品を集めて組み立てる前提のオープンハードウェアです
- _完成品の同人ショップでの委託版版もするかもしれません_

## Required Parts

- RaspberryPi Pico (SoC)
- ILI9341 with Touch Module (LCD module)
- UDA1334A (DAC module)
- ブレッドボード
- ジャンパー線♂♀ x 17本
  - 内訳:
    - LCD 接続 = 8本
    - LCD タッチパネル接続 = 4本
    - DAC 接続 = 5本

## PIN assign

### ILI9341 (LCD Module)

|RaspberryPi Pico|ILI9341 (LCD Module)|
|:-|:-|
|`3.3V`|`VCC`|
|`GPIO27`|`LED`|
|`GND`|`GND`|
|`GPIO17 (SPI0 CS)`|`CS`|
|`GPIO22`|`RESET`|
|`GPIO28`|`DC`|
|`GPIO19 (SPI0 TX)`|`SDI (MOSI)`|
|`GPIO18 (SPI0 SCK)`|`SCK`|
|`GPIO20`|`T_CS`|
|`GPIO16 (SPI0 MISO)`|`T_DO (MISO)`|
|`GPIO18 (SPI0 SCK)`|`T_CLK`|
|`GPIO19 (SPI0 MOSI)`|`T_DIN (MOSI)`|

### UDA1334A (DAC Module)

TODO: あとで書く

## License

本プログラムのライセンスは [GPLv3](LICENSE.txt) とします。

また、本プログラムには以下のソフトウェアに依存しているため、再配布時にはそれぞれのライセンス条項の遵守をお願いいたします。

- TFT eSPI
    - Web Site: [https://github.com/Bodmer/TFT_eSPI](https://github.com/Bodmer/TFT_eSPI)
    - License: [Combined Licenses](./licenses-copy/TFT_eSPI.txt)
    - `Copyright (c) 2012 Adafruit Industries.  All rights reserved.`
    - `Copyright (c) 2023 Bodmer (https://github.com/Bodmer)`
