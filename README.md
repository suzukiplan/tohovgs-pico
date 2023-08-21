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

## How to Build

### Build Support OS

- Linux
- macOS

- ビルドに iconv コマンドが必要なため UNIX と互換性のある OS のビルドのみサポートしています（Windows でのビルドをサポートする予定はないので Linux または macOS を準備してください）
- Linux は Ubuntu でビルド確認しています（おそらく Debian や Raspberry Pi Desktop 等でのビルドも可能だと思われます）

### Required Middleware

- [PlatformIO Core](https://github.com/platformio/platformio-core)
- GNU Make
- GCC C/C++

### How to Build

```
make
```

## License

本プログラムのライセンスは [GPLv3](LICENSE.txt) とします。

また、本プログラムでは以下の Open Source Software や素材を利用しています。

- miniz
  - Web Site: [https://github.com/richgel999/miniz](https://github.com/richgel999/miniz)
  - License: [MIT](./licenses-copy/miniz.txt)
  - `Copyright 2013-2014 RAD Game Tools and Valve Software`
  - `Copyright 2010-2014 Rich Geldreich and Tenacious Software LLC`
- TFT eSPI
  - Web Site: [https://github.com/Bodmer/TFT_eSPI](https://github.com/Bodmer/TFT_eSPI)
  - License: [Combined Licenses](./licenses-copy/TFT_eSPI.txt)
  - `Copyright (c) 2012 Adafruit Industries.  All rights reserved.`
  - `Copyright (c) 2023 Bodmer (https://github.com/Bodmer)`
- 8×12 ドット日本語フォント「k8x12」
  - Web Site: [https://littlelimit.net/k8x12.htm](https://littlelimit.net/k8x12.htm)
  - License: [Original](./licenses-copy/k8x12.txt)
