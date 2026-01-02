# NetworkCamera_CoreS3

M5Stack CoreS3をQVR Surveillanceに接続可能なネットワークカメラ（RTSPサーバー）として動作させるプロジェクトです。

## 概要

このプロジェクトは、M5Stack CoreS3の内蔵カメラ（GC0308）を使用して、RTSP（Real-Time Streaming Protocol）サーバーを立ち上げ、QNAPのQVR Surveillanceから「汎用RTSPカメラ」として認識させることができます。

## 機能

- RTSPプロトコルによる映像ストリーミング
- WiFi接続（設定ファイルから読み込み）
- M5Stack CoreS3の内蔵カメラを使用
- QVR Surveillance対応
- 設定ファイルによる柔軟な設定

## 必要な環境

- PlatformIO
- M5Stack CoreS3
- WiFi環境

## セットアップ手順

### 1. リポジトリのクローン

```bash
git clone <repository-url>
cd NetworkCamera_CoreS3
```

### 2. 設定ファイルの作成

`setting.yaml.example`をコピーして`setting.yaml`を作成し、WiFi情報を設定してください。

```bash
# Windowsの場合
copy setting.yaml.example setting.yaml

# Linux/Macの場合
cp setting.yaml.example setting.yaml
```

`setting.yaml`を編集して、WiFiのSSIDとパスワードを設定します：

```yaml
wifi:
  ssid: "your_wifi_ssid"
  password: "your_wifi_password"

rtsp:
  port: 8554
  stream_path: "/mjpeg/1"

camera:
  frame_size: 5
  jpeg_quality: 12
  fps: 10
```

**重要**: `setting.yaml`は`.gitignore`に含まれているため、GitHubには公開されません。

### 3. 設定ファイルのアップロード

PlatformIOを使用して、設定ファイルをM5Stack CoreS3のLittleFSにアップロードします。

```bash
# PlatformIO CLIを使用する場合
pio run --target uploadfs

# または、PlatformIO IDEの「Upload Filesystem Image」を使用
```

### 4. ビルドとアップロード

```bash
# ビルド
pio run

# アップロード
pio run --target upload

# シリアルモニター
pio device monitor
```

## 使用方法

### 1. 起動

M5Stack CoreS3に電源を接続すると、自動的に以下が実行されます：

1. WiFi接続
2. カメラ初期化
3. RTSPサーバー起動

シリアルモニターで以下のような情報が表示されます：

```
=== M5Stack CoreS3 ネットワークカメラ ===
WiFi接続を開始します...
SSID: your_wifi_ssid
WiFi接続成功!
IPアドレス: 192.168.1.100
RTSP URL: rtsp://192.168.1.100:8554/mjpeg/1
```

### 2. QVR Surveillanceでの設定

1. QVR Surveillanceを開く
2. **カメラ設定** → **追加** を選択
3. **手動でカメラを追加** を選択
4. **ブランド**: `Generic`（または `User-defined`）を選択
5. **モデル**: `Generic RTSP` を選択
6. **RTSP URL**: `rtsp://[M5StackのIPアドレス]:8554/mjpeg/1` を入力
   - 例: `rtsp://192.168.1.100:8554/mjpeg/1`
7. **ユーザー名/パスワード**: 空欄（認証なしの場合）

### 3. VLCでの動作確認

PCからVLCメディアプレーヤーを使用して、RTSPストリームを確認できます：

1. VLCを開く
2. **メディア** → **ネットワークストリームを開く**
3. URLに `rtsp://[M5StackのIPアドレス]:8554/mjpeg/1` を入力
4. **再生**をクリック

## 設定項目

### WiFi設定

- `wifi.ssid`: WiFiのSSID
- `wifi.password`: WiFiのパスワード

### RTSP設定

- `rtsp.port`: RTSPサーバーのポート番号（デフォルト: 8554）
- `rtsp.stream_path`: ストリームパス（デフォルト: `/mjpeg/1`）

### カメラ設定

- `camera.frame_size`: フレームサイズ
  - 0: QQVGA (160x120)
  - 1: HQVGA (240x176)
  - 2: QVGA (320x240)
  - 3: CIF (400x296)
  - 4: VGA (640x480)
  - 5: SVGA (800x600) - 推奨
  - 6: XGA (1024x768)
  - 7: SXGA (1280x1024)
  - 8: UXGA (1600x1200)
- `camera.jpeg_quality`: JPEG品質（0-63、低いほど高品質、デフォルト: 12）
- `camera.fps`: 目標フレームレート（デフォルト: 10）

## 注意事項

### RTSPライブラリの実装について

現在の実装では、Micro-RTSPライブラリの実際のAPI構造に合わせて、RTSP機能の実装がコメントアウトされています。

**次のステップ：**

1. ビルドが成功したら、Micro-RTSPライブラリの実際のヘッダーファイル名を確認してください
2. `src/main.cpp`の`#include`文を実際のヘッダーファイル名に合わせて修正してください
3. RTSPストリーマーの初期化とループ処理のコードを、実際のライブラリAPIに合わせて実装してください

**参考：**
- Micro-RTSPライブラリのGitHubリポジトリ: https://github.com/geeksville/Micro-RTSP
- ライブラリのサンプルコードを確認して、正しい実装方法を参照してください

## トラブルシューティング

### WiFi接続に失敗する

- `setting.yaml`のSSIDとパスワードが正しいか確認してください
- WiFiの信号強度を確認してください
- シリアルモニターでエラーメッセージを確認してください

### カメラが初期化できない

- M5Stack CoreS3のカメラが正しく接続されているか確認してください
- シリアルモニターでエラーメッセージを確認してください

### RTSPストリームが表示されない

- M5Stack CoreS3とPC/NASが同じネットワーク上にあるか確認してください
- ファイアウォールの設定を確認してください
- RTSP URLが正しいか確認してください

### 設定ファイルが読み込めない

- `setting.yaml`がLittleFSに正しくアップロードされているか確認してください
- ファイル名が`setting.yaml`または`setting.txt`であることを確認してください
- PlatformIOの「Upload Filesystem Image」を実行してください

## 開発のヒント

- **解像度の選択**: ESP32-S3はパワフルですが、高解像度（VGA以上）でフレームレートを上げるとWi-Fi帯域やメモリを圧迫します。まずはQVGA (320x240)程度から始め、安定性を確認しながら上げるのがコツです。
- **熱対策**: 常時ストリーミングを行うとCoreS3がかなり熱を持ちます。画面（LCD）の輝度を落とすか、不要なら消灯させるコードを追加して消費電力を抑えてください。
- **給電**: バッテリー駆動では数時間しか持ちません。USB-Cからの常時給電が必要です。
- **フレームレート**: QVR Surveillance側で「接続切れ」と判断されないよう、最低でも5fps程度は維持できるように調整してください。

## ライセンス

このプロジェクトのライセンス情報をここに記載してください。

## 参考資料

- [M5Stack CoreS3 公式ドキュメント](https://docs.m5stack.com/)
- [Micro-RTSP ライブラリ](https://github.com/geeksville/Micro-RTSP)
- [QVR Surveillance マニュアル](https://www.qnap.com/ja-jp/software/qvr-surveillance)
