/**
 * @file ConfigManager.cpp
 * @brief ConfigManagerクラスの実装
 */

#include "ConfigManager.h"

ConfigManager::ConfigManager() {
}

bool ConfigManager::begin() {
    // LittleFSをマウント
    if (!LittleFS.begin(true)) {
        Serial.println("LittleFSマウントに失敗しました");
        return false;
    }
    Serial.println("LittleFSマウント成功");
    return true;
}

bool ConfigManager::configExists() {
    // setting.yaml または setting.txt の存在確認
    return LittleFS.exists("/setting.yaml") || LittleFS.exists("/setting.txt");
}

bool ConfigManager::loadConfig(WiFiConfig& config) {
    // 設定ファイルを読み込む（setting.yamlを優先、なければsetting.txt）
    File file;
    if (LittleFS.exists("/setting.yaml")) {
        file = LittleFS.open("/setting.yaml", "r");
        Serial.println("setting.yamlを読み込みます");
    } else if (LittleFS.exists("/setting.txt")) {
        file = LittleFS.open("/setting.txt", "r");
        Serial.println("setting.txtを読み込みます");
    } else {
        Serial.println("設定ファイルが見つかりません（setting.yaml または setting.txt）");
        return false;
    }

    if (!file) {
        Serial.println("設定ファイルを開けませんでした");
        return false;
    }

    // ファイル内容を読み込む
    configContent = "";
    while (file.available()) {
        configContent += char(file.read());
    }
    file.close();

    // 設定値を取得
    config.ssid = getValue("wifi.ssid", "");
    config.password = getValue("wifi.password", "");
    config.rtspPort = getIntValue("rtsp.port", 8554);
    config.streamPath = getValue("rtsp.stream_path", "/mjpeg/1");
    config.frameSize = getIntValue("camera.frame_size", 5);
    config.jpegQuality = getIntValue("camera.jpeg_quality", 12);
    config.fps = getIntValue("camera.fps", 10);

    // WiFi設定の必須チェック
    if (config.ssid.length() == 0) {
        Serial.println("エラー: wifi.ssidが設定されていません");
        return false;
    }

    Serial.println("設定ファイルの読み込みが完了しました");
    Serial.printf("SSID: %s\n", config.ssid.c_str());
    Serial.printf("RTSP Port: %d\n", config.rtspPort);
    Serial.printf("Stream Path: %s\n", config.streamPath.c_str());

    return true;
}

String ConfigManager::trim(String str) {
    str.trim();
    return str;
}

String ConfigManager::getValue(const String& key, const String& defaultValue) {
    int startPos = 0;
    String searchKey = key + ":";
    
    // キーを検索（コロン区切りを想定）
    int keyPos = configContent.indexOf(searchKey, startPos);
    if (keyPos == -1) {
        // コロンなしでも検索
        searchKey = key + "=";
        keyPos = configContent.indexOf(searchKey, startPos);
    }
    
    if (keyPos == -1) {
        return defaultValue;
    }

    // 値の開始位置を取得
    int valueStart = keyPos + searchKey.length();
    
    // 値の終了位置を取得（改行または#まで）
    int valueEnd = configContent.indexOf('\n', valueStart);
    int commentPos = configContent.indexOf('#', valueStart);
    if (commentPos != -1 && (valueEnd == -1 || commentPos < valueEnd)) {
        valueEnd = commentPos;
    }
    if (valueEnd == -1) {
        valueEnd = configContent.length();
    }

    // 値を抽出
    String value = configContent.substring(valueStart, valueEnd);
    value = trim(value);
    
    // クォートを削除
    if (value.startsWith("\"") && value.endsWith("\"")) {
        value = value.substring(1, value.length() - 1);
    } else if (value.startsWith("'") && value.endsWith("'")) {
        value = value.substring(1, value.length() - 1);
    }

    return value;
}

int ConfigManager::getIntValue(const String& key, int defaultValue) {
    String value = getValue(key, String(defaultValue));
    if (value.length() == 0) {
        return defaultValue;
    }
    return value.toInt();
}

