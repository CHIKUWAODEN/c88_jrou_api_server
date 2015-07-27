= 参考資料

今回参考にしたWebサイトなどを紹介します.

: @<href>{http://golang.org}
  いわずとしれたGoのオフィシャルサイトです.
  特にEffective Goというコンテンツは、知見に満ちあふれているのでGoを学ぶのであれば必読でしょう.
: @<href>{http://leveldb.org/}
  LevelDBのオフィシャルサイト.
  LevelDBそのものについての知識からすくっていきたい場合は、こちらからチェックしていきましょう.

本書で実装したサーバでは、次のライブラリを利用させて頂いています.
この場を借りて、各ライブラリの作者様にお礼申し上げる次第です.

: @<href>{https://github.com/syndtr/goleveldb}
  LevelDBをGoからあつかうためのライブラリ.
  データベースまわりの実装に使用しました.
: @<href>{https://github.com/julienschmidt/httprouter}
  標準のhttpパッケージと組み合わせて利用できる、ルーティングライブラリ.
  リクエストされたURIに応じて内部的に呼び出す処理を振り分けるために利用しています.
: @<href>{https://github.com/pborman/uuid}
  UUIDを生成するためのライブラリ.
  今回はレポートAPIにおいて、個々のレポートに付与するUUIDを生成するために使用しています.

