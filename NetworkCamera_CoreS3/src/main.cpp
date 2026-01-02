/**
 * @file main.cpp
 * @brief M5Stack CoreS3 ネットワークカメラ（RTSPサーバー）
 * 
 * M5Stack CoreS3をQVR Surveillanceに接続可能なネットワークカメラとして動作させます。
 * RTSPプロトコルで映像をストリーミングします。
 */

#include <M5Unified.h>
#include <WiFi.h>
#include "esp_camera.h"
#include "ConfigManager.h"

// Micro-RTSPライブラリのヘッダー
// 実際のライブラリ構造に合わせて調整が必要な場合があります
// ビルドエラーを確認して、正しいヘッダーファイル名を使用してください
// #include "OV2640Streamer.h"
// #include "CRtspSession.h"

// グローバル変数
WiFiServer rtspServer(8554);
ConfigManager configManager;
WiFiConfig wifiConfig;

// カメラ設定
camera_fb_t* fb = nullptr;

/**
 * @brief WiFi接続処理
 * @return 成功時true
 */
bool connectWiFi() {
    Serial.printf("WiFi接続を開始します...\n");
    Serial.printf("SSID: %s\n", wifiConfig.ssid.c_str());
    
    WiFi.mode(WIFI_STA);
    WiFi.begin(wifiConfig.ssid.c_str(), wifiConfig.password.c_str());
    
    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 30) {
        delay(500);
        Serial.print(".");
        attempts++;
    }
    Serial.println();
    
    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("WiFi接続成功!");
        Serial.printf("IPアドレス: %s\n", WiFi.localIP().toString().c_str());
        Serial.printf("RTSP URL: rtsp://%s:%d%s\n", 
                     WiFi.localIP().toString().c_str(), 
                     wifiConfig.rtspPort, 
                     wifiConfig.streamPath.c_str());
        return true;
    } else {
        Serial.println("WiFi接続に失敗しました");
        return false;
    }
}

/**
 * @brief カメラ初期化
 * @return 成功時true
 */
bool initCamera() {
    Serial.println("カメラを初期化しています...");
    
    // M5Stack CoreS3のカメラピン設定
    // GC0308カメラのピン設定
    camera_config_t config;
    config.pin_pwdn = -1;
    config.pin_reset = -1;
    config.pin_xclk = 15;
    config.pin_sscb_sda = 4;
    config.pin_sscb_scl = 5;
    config.pin_d7 = 16;
    config.pin_d6 = 17;
    config.pin_d5 = 18;
    config.pin_d4 = 12;
    config.pin_d3 = 10;
    config.pin_d2 = 8;
    config.pin_d1 = 9;
    config.pin_d0 = 11;
    config.pin_vsync = 6;
    config.pin_href = 7;
    config.pin_pclk = 13;
    
    // カメラ設定
    config.xclk_freq_hz = 20000000;  // 20MHz
    config.ledc_timer = LEDC_TIMER_0;
    config.ledc_channel = LEDC_CHANNEL_0;
    config.pixel_format = PIXFORMAT_JPEG;
    config.frame_size = (framesize_t)wifiConfig.frameSize;
    config.jpeg_quality = wifiConfig.jpegQuality;
    config.fb_count = 2;
    config.grab_mode = CAMERA_GRAB_WHEN_EMPTY;
    config.fb_location = CAMERA_FB_IN_PSRAM;
    
    // カメラ初期化
    esp_err_t err = esp_camera_init(&config);
    if (err != ESP_OK) {
        Serial.printf("カメラの初期化に失敗しました: 0x%x\n", err);
        return false;
    }
    
    // カメラ設定を取得して確認
    sensor_t* s = esp_camera_sensor_get();
    if (s != nullptr) {
        // フレームサイズとJPEG品質を設定
        s->set_framesize(s, (framesize_t)wifiConfig.frameSize);
        s->set_quality(s, wifiConfig.jpegQuality);
    }
    
    Serial.println("カメラの初期化が完了しました");
    return true;
}

/**
 * @brief RTSPサーバーの初期化
 * @return 成功時true
 */
bool initRTSP() {
    Serial.println("RTSPサーバーを初期化しています...");
    
    // RTSPサーバーを開始
    rtspServer.begin();
    
    // ストリーマーの作成
    // 注意: Micro-RTSPライブラリの実際のAPIに合わせて調整が必要な場合があります
    // 実際のライブラリの構造を確認して、正しい実装を追加してください
    // 例: streamer = new OV2640Streamer(&rtspServer);
    
    Serial.printf("RTSPサーバーがポート %d で待機中です\n", wifiConfig.rtspPort);
    Serial.println("注意: RTSPストリーマーの実装は、実際のライブラリAPIに合わせて調整が必要です");
    Serial.println("Micro-RTSPライブラリの実際のヘッダーファイル名を確認して、#includeを修正してください");
    return true;
}

/**
 * @brief セットアップ処理
 */
void setup() {
    // M5Stack初期化
    auto cfg = M5.config();
    M5.begin(cfg);
    
    // シリアル通信開始
    Serial.begin(115200);
    delay(1000);
    Serial.println("\n\n=== M5Stack CoreS3 ネットワークカメラ ===");
    
    // 画面表示
    M5.Lcd.setRotation(1);
    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.setTextColor(WHITE);
    M5.Lcd.setTextSize(2);
    M5.Lcd.setCursor(10, 10);
    M5.Lcd.println("Network Camera");
    M5.Lcd.setTextSize(1);
    
    // 設定ファイル読み込み
    if (!configManager.begin()) {
        Serial.println("設定マネージャーの初期化に失敗しました");
        M5.Lcd.setCursor(10, 40);
        M5.Lcd.println("Config Error!");
        while (1) delay(1000);
    }
    
    if (!configManager.loadConfig(wifiConfig)) {
        Serial.println("設定ファイルの読み込みに失敗しました");
        Serial.println("setting.yaml または setting.txt を作成してください");
        M5.Lcd.setCursor(10, 40);
        M5.Lcd.println("No Config File!");
        M5.Lcd.setCursor(10, 55);
        M5.Lcd.println("Create setting.yaml");
        while (1) delay(1000);
    }
    
    // WiFi接続
    M5.Lcd.setCursor(10, 40);
    M5.Lcd.println("Connecting WiFi...");
    if (!connectWiFi()) {
        Serial.println("WiFi接続に失敗しました。再起動してください。");
        M5.Lcd.setCursor(10, 55);
        M5.Lcd.println("WiFi Failed!");
        while (1) delay(1000);
    }
    
    // カメラ初期化
    M5.Lcd.setCursor(10, 70);
    M5.Lcd.println("Init Camera...");
    if (!initCamera()) {
        Serial.println("カメラの初期化に失敗しました");
        M5.Lcd.setCursor(10, 85);
        M5.Lcd.println("Camera Error!");
        while (1) delay(1000);
    }
    
    // RTSPサーバー初期化
    M5.Lcd.setCursor(10, 100);
    M5.Lcd.println("Init RTSP...");
    if (!initRTSP()) {
        Serial.println("RTSPサーバーの初期化に失敗しました");
        M5.Lcd.setCursor(10, 115);
        M5.Lcd.println("RTSP Error!");
        while (1) delay(1000);
    }
    
    // 初期化完了
    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.setCursor(10, 10);
    M5.Lcd.setTextSize(2);
    M5.Lcd.println("Ready!");
    M5.Lcd.setTextSize(1);
    M5.Lcd.setCursor(10, 40);
    M5.Lcd.printf("IP: %s", WiFi.localIP().toString().c_str());
    M5.Lcd.setCursor(10, 55);
    M5.Lcd.printf("RTSP: %d", wifiConfig.rtspPort);
    M5.Lcd.setCursor(10, 70);
    M5.Lcd.println("Streaming...");
    
    Serial.println("=== 初期化完了 ===");
    Serial.printf("RTSP URL: rtsp://%s:%d%s\n", 
                 WiFi.localIP().toString().c_str(), 
                 wifiConfig.rtspPort, 
                 wifiConfig.streamPath.c_str());
}

/**
 * @brief メインループ
 */
void loop() {
    M5.update();
    
    // RTSPクライアントの接続処理
    WiFiClient client = rtspServer.available();
    if (client) {
        Serial.println("新しいRTSPクライアントが接続しました");
        
        // ストリーミングセッションの処理
        // 注意: Micro-RTSPライブラリの実際のAPIに合わせて調整が必要です
        // 実際のライブラリの構造を確認して、正しい実装を追加してください
        // 例: if (streamer != nullptr) {
        //     streamer->handleRequests(0);
        // }
    }
    
    // フレームレート制御
    static uint32_t lastFrameTime = 0;
    uint32_t frameInterval = 1000 / wifiConfig.fps;  // ミリ秒
    
    if (millis() - lastFrameTime >= frameInterval) {
        // カメラからフレームを取得
        fb = esp_camera_fb_get();
        if (fb != nullptr) {
            // フレームをストリーミング
            // 注意: 実際の実装は使用するライブラリのAPIに依存します
            // 実際のライブラリの構造に合わせて修正してください
            
            // フレームバッファを解放
            esp_camera_fb_return(fb);
            fb = nullptr;
        }
        lastFrameTime = millis();
    }
    
    // 短い遅延でCPU負荷を軽減
    delay(10);
}
