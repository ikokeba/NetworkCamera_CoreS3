/**
 * @file ConfigManager.h
 * @brief WiFi設定ファイル読み込み用のユーティリティクラス
 * 
 * setting.yaml または setting.txt からWiFi設定を読み込みます。
 * シンプルなキー=値形式をサポートします。
 */

#ifndef CONFIG_MANAGER_H
#define CONFIG_MANAGER_H

#include <Arduino.h>
#include <FS.h>
#include <LittleFS.h>

/**
 * @brief 設定情報を保持する構造体
 */
struct WiFiConfig {
    String ssid;
    String password;
    int rtspPort = 8554;
    String streamPath = "/mjpeg/1";
    int frameSize = 5;        // SVGA (800x600)
    int jpegQuality = 12;
    int fps = 10;
};

/**
 * @brief 設定ファイル管理クラス
 */
class ConfigManager {
public:
    /**
     * @brief コンストラクタ
     */
    ConfigManager();

    /**
     * @brief 初期化（ファイルシステムのマウント）
     * @return 成功時true
     */
    bool begin();

    /**
     * @brief 設定ファイルを読み込む
     * @param config 設定を格納する構造体への参照
     * @return 成功時true
     */
    bool loadConfig(WiFiConfig& config);

    /**
     * @brief 設定ファイルの存在確認
     * @return 存在する場合true
     */
    bool configExists();

private:
    /**
     * @brief 文字列のトリム（前後の空白を削除）
     * @param str トリムする文字列
     * @return トリム後の文字列
     */
    String trim(String str);

    /**
     * @brief 設定ファイルから値を取得
     * @param key キー名
     * @param defaultValue デフォルト値
     * @return 設定値（見つからない場合はデフォルト値）
     */
    String getValue(const String& key, const String& defaultValue = "");

    /**
     * @brief 設定ファイルから整数値を取得
     * @param key キー名
     * @param defaultValue デフォルト値
     * @return 設定値（見つからない場合はデフォルト値）
     */
    int getIntValue(const String& key, int defaultValue = 0);

    String configContent;  // 設定ファイルの内容を保持
};

#endif // CONFIG_MANAGER_H

