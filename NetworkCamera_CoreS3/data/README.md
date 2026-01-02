# data ディレクトリ

このディレクトリには、M5Stack CoreS3のLittleFSファイルシステムにアップロードするファイルを配置します。

## 設定ファイルのアップロード方法

1. `setting.yaml.example`をコピーして`setting.yaml`を作成します
2. `setting.yaml`を編集して、WiFiのSSIDとパスワードを設定します
3. PlatformIOで「Upload Filesystem Image」を実行します

## 注意事項

- `setting.yaml`は機密情報を含むため、GitHubには公開されません（`.gitignore`で除外されています）
- `setting.yaml.example`はテンプレートとして公開されています

