# 東方BGM on VGS for RaspberryPi Pico

## WIP Status

まだ、ほぼほぼ何もできてません

## About

- 本リポジトリでは、同人ハードウェア版 東方BGM on VGS のファームウェアと組み立て手順を提供します
- 同人ハードウェア版 東方BGM on VGS は基本的に、各自で部品を集めて組み立てる前提のオープンハードウェアです
- _完成品の同人ショップでの委託版版もするかもしれません_
- _同人流通の範囲内であれば東方VGS（実機版）を自由に制作＆販売できるようにするつもりです_

## Required Parts and Tools

電子工作初心者でも実装可能なもっとも簡単な実装に必要な部品と工具を示します。

### Parts

__（国内で調達できない部品）__

- [RP2040: Type-C 16MB (SoC module)](https://ja.aliexpress.com/item/1005004005660504.html)
  - RaspberryPi Pico の開発ボードの互換製品です
  - 国内でも調達できる純正の RaspberryPi Pico だとフラッシュ容量不足のため全曲を入れることはできません
- [ILI9341: Module with Touch (LCD module)](https://ja.aliexpress.com/item/1005003005413104.html)
  - このタッチパネルは抵抗膜方式なので、操作性は良くありません
  - 予算に余裕があれば静電容量方式のタッチパネルを調達することをお勧めします
- [UDA1334A (DAC module)](https://ja.aliexpress.com/item/1005001993192815.html)
  - UDA1334A は EOL (生産終了) なので純正品の調達は現在できません
  - 上記リンクは互換製品です
  - UDA1334A でなくても I2S インタフェースの DAC であれば恐らく問題ありません

__（国内で調達可能な部品）__

- [ブレッドボード](https://www.marutsu.co.jp/pc/i/14660/)
- [ジャンパー線♂♀](https://www.marutsu.co.jp/pc/i/69682/) x 17本

### Tools

- ハンダゴテ
- ハンダゴテ台
- ハンダ

> 上記の工具は、RP2040 と UDA1334A のピンヘッダを取り付けに必要です。

## PIN assign

ブレッドボードとジャンパー線を用いて、各モジュールを下表のように配線してください。

|RP2040 (SoC)|ILI9341 (LCD)|UDA1334A (DAC)|
|:-|:-|:-|
|17: `GPIO13`|-|`DIN`|
|18: `GND`|-|`GND`|
|19: `GPIO14`|-|`BCLK`|
|20: `GPIO15`|-|`WSEL`|
|21: `GPIO16`|`T_DO (MISO)`|-|
|22: `GPIO17`|`CS`|-|
|24: `GPIO18`|`SCK`, `T_CLK`|-|
|25: `GPIO19`|`SDI (MOSI)`, `T_DIN (MOSI)`|-|
|26: `GPIO20`|`T_CS`|-|
|29: `GPIO22`|`RESET`|-|
|32: `GPIO27`|`LED`|-|
|34: `GPIO28`|`DC`|-|
|36: `3.3V`|`VCC`|-|
|38: `GND`|`GND`|-|
|40: `VOUT (5V)`|-|`VIN`|

## How to Build

### Build Support OS

- Linux
- macOS

> - ビルドに iconv コマンドが必要なため UNIX と互換性のある OS のビルドのみサポートしています（Windows でのビルドをサポートする予定はないので Linux または macOS を準備してください）
> - Linux は Ubuntu でビルド確認しています（おそらく Debian や Raspberry Pi Desktop 等でのビルドも可能だと思われます）

### Required Middleware

- [PlatformIO Core](https://github.com/platformio/platformio-core)
- GNU Make
- GCC C/C++

### How to Build

RP2040 の BOOT ボタンを押しながら PC へ USB 接続後、以下のコマンドを実行すればビルドとフラッシュの書き込みが実行されます。

```bash
make
```

フラッシュの書き込みをせずにビルドのみ行いたい場合は `build` オプションを指定してください。

```bash
make build
```

## License

本プログラムのライセンスは [GPLv3](LICENSE.txt) とします。

また、本プログラムでは以下の Open Source Software や素材を利用しています。

- LZ4 Library
  - Web Site: [https://github.com/lz4/lz4](https://github.com/lz4/lz4) - [lib](https://github.com/lz4/lz4/tree/dev/lib)
  - License: [2-Clause BSD](./licenses-copy/lz4-library.txt)
  - `Copyright (c) 2011-2020, Yann Collet`
- TFT eSPI
  - Web Site: [https://github.com/Bodmer/TFT_eSPI](https://github.com/Bodmer/TFT_eSPI)
  - License: [Combined Licenses](./licenses-copy/TFT_eSPI.txt)
  - `Copyright (c) 2012 Adafruit Industries.  All rights reserved.`
  - `Copyright (c) 2023 Bodmer (https://github.com/Bodmer)`
- 8×12 ドット日本語フォント「k8x12」
  - Web Site: [https://littlelimit.net/k8x12.htm](https://littlelimit.net/k8x12.htm)
  - License: [Original](./licenses-copy/k8x12.txt)
