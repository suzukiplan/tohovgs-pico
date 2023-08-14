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

本プログラムのライセンスは [MIT](LICENSE.txt) とします。

また、本プログラムには以下のソフトウェアに依存しているため、再配布時にはそれぞれのライセンス条項の遵守をお願いいたします。

- LovyanGFX
    - Web Site: [https://github.com/lovyan03/LovyanGFX](https://github.com/lovyan03/LovyanGFX)
    - License: [FreeBSD](./licenses-copy/LovyanGFX.txt)
    - `Copyright (c) 2012 Adafruit Industries.  All rights reserved.`
    - `Copyright (c) 2020 Bodmer (https://github.com/Bodmer)`
    - `Copyright (c) 2020 lovyan03 (https://github.com/lovyan03)`
- TJpgDec
    - Web Site: [http://elm-chan.org/fsw/tjpgd/00index.html](http://elm-chan.org/fsw/tjpgd/00index.html)
    - License: [original](./licenses-copy/TJpgDec.txt)
    - `(C)ChaN, 2019`
- Pngle
    - Web Site: [https://github.com/kikuchan/pngle](https://github.com/kikuchan/pngle)
    - License: [MIT](./licenses-copy/Pngle.txt)
    - `Copyright (c) 2019 kikuchan`
- QRCode
    - Web Site: [https://github.com/ricmoo/QRCode](https://github.com/ricmoo/QRCode)
    - License: [MIT](./licenses-copy/QRCode.txt)
    - `Copyright (c) 2017 Richard Moore     (https://github.com/ricmoo/QRCode)`
    - `Copyright (c) 2017 Project Nayuki    (https://www.nayuki.io/page/qr-code-generator-library)`
- result
    - Web Site: [https://github.com/bitwizeshift/result](https://github.com/bitwizeshift/result)
    - License: [MIT](./licenses-copy/result.txt)
    - `Copyright (c) 2017-2021 Matthew Rodusek`
- GFX font and GLCD font
    - Web Site: [https://github.com/adafruit/Adafruit-GFX-Library](https://github.com/adafruit/Adafruit-GFX-Library)
    - License: [2-clause BSD](./licenses-copy/Adafruit-GFX-Library.txt)
    - `Copyright (c) 2012 Adafruit Industries.  All rights reserved.`
- Font 2,4,6,7,8
    - Web Site: [https://github.com/Bodmer/TFT_eSPI](https://github.com/Bodmer/TFT_eSPI)
    - License: [FreeBSD](./licenses-copy/TFT_eSPI.txt)
    - `Copyright (c) 2012 Adafruit Industries.  All rights reserved.`
    - `Copyright (c) 2023 Bodmer (https://github.com/Bodmer)`
- converted IPA font
    - Web Site: [https://www.ipa.go.jp/index.html](https://www.ipa.go.jp/index.html)
    - License: [IPA Font License Agreement v1.0](./licenses-copy/IPA_Font_License_Agreement_v1.0.txt)
- efont
    - Web Site: [http://openlab.ring.gr.jp/efont/](http://openlab.ring.gr.jp/efont/)
    - License: [3-clause BSD](./licenses-copy/efont.txt)
    - `(c) Copyright 2000-2001 /efont/ The Electronic Font Open Laboratory. All rights reserved.`
- TomThumb font
    - Web Site:
    - License: [3-clause BSD](./licenses-copy/TomThumb.txt)
    - `Copyright 1999 Brian J. Swetland`
    - `Copyright 1999 Vassilii Khachaturov`
    - `Portions (of vt100.c/vt100.h) copyright Dan Marks`
