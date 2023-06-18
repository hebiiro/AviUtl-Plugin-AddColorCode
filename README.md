# AviUtl プラグイン - カラーコード追加

色の選択ダイアログでカラーコードを使えるようにします。<br>
[最新バージョンをダウンロード](../../releases/latest/)

![カラーコード](https://user-images.githubusercontent.com/96464759/171362721-9d5ef42b-986d-40c1-b46e-9d6a2262d65b.png)

## ダウンロード

1. ページ右側にある [Releases](/../../releases) をクリックして移動します。
2. ダウンロードしたいバージョンをクリックして移動します。
3. Assets の下に圧縮ファイルがあるのでクリックしてダウンロードします。

## 導入方法

1. 以下のファイルを AviUtl の Plugins フォルダに配置します。
	* AddColorCode.auf
	* AddColorCode.ini

## 使用方法

1. 色の選択ダイアログを表示します。
2. スポイトの横にエディットボックスが追加されているので、そこにカラーコードを入力します。
* 先頭の # は無視されます。
* rrggbb か rgb 形式の16進数文字列を指定できます。

## 設定方法

AddColorCode.ini をテキストエディタで編集してから AviUtl を起動します。

```ini
[Settings]
offset.x=0 ; カラーピッカーの位置に与えるオフセットを指定します。
offset.y=0
```

## 更新履歴

* 1.2.1 - 2023/06/18 スポイトツールが正常に動作しない問題を修正
* 1.2.0 - 2023/05/31 カラーピッカーの位置を変更できる機能を追加
* 1.1.0 - 2022/06/01 カラーコードの変更が反映されない問題を修正
* 1.0.0 - 2022/06/01 初版

## 動作確認

* (必須) AviUtl 1.10 & 拡張編集 0.92 http://spring-fragrance.mints.ne.jp/aviutl/
* (共存確認) patch.aul r42 https://scrapbox.io/ePi5131/patch.aul

## クレジット

* Microsoft Research Detours Package https://github.com/microsoft/Detours
* aviutl_exedit_sdk https://github.com/ePi5131/aviutl_exedit_sdk
* Common Library https://github.com/hebiiro/Common-Library

## 作成者情報

* 作成者 - 蛇色 (へびいろ)
* GitHub - https://github.com/hebiiro
* Twitter - https://twitter.com/io_hebiiro

## 免責事項

この作成物および同梱物を使用したことによって生じたすべての障害・損害・不具合等に関しては、私と私の関係者および私の所属するいかなる団体・組織とも、一切の責任を負いません。各自の責任においてご使用ください。
