# Simulator for PC

PC (macOS or Linux) で東方VGS実機版の動作をシミュレートします

## Build and Execute

### macOS

XCODE と HomeBrew がインストールされた環境のターミナルで以下のコマンドを実行することでビルドと実行ができます。

```bash
# SDL2 をインストール
brew install sdl2

# リポジトリを取得
git clone https://github.com/suzukiplan/tohovgs-pico

# simulator ディレクトリへ移動
cd tohovgs-pico/simulator

# ビルド
make

# 実行
./app
```

### Linux (Ubuntu)

gcc と gnu make がインストールされた環境のターミナルで以下のコマンドを実行することでビルドと実行ができます。

```bash
# SDL2 をインストール
sudo apt-get install libsdl2-dev

# ALSA をインストール
sudo apt-get install libasound2
sudo apt-get install libasound2-dev

# リポジトリを取得
git clone https://github.com/suzukiplan/tohovgs-pico

# simulator ディレクトリへ移動
cd tohovgs-pico/simulator

# ビルド
make

# 実行
./app
```
