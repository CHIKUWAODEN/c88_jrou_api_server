= 実装を考える

//read{
設計がみえてきたので、具体的なコードに落としこんでいきます.
この章では、実際に作成した「ラーメン二郎WebAPIサーバ jirou」の具体的な実装について、
そのポイントごとにコードなどを交えつつ、つらつらと紹介していきます.
//}


== ラーメン二郎Web APIサーバ jirou

=== 名前is重要andあると便利

何かしらの名前がないと文章が書きづらいので、作成するプログラムを「jirou」とし、以降の文章ではこの呼称を用いるものとしています.
また、jirouに関連するコードはすべて、下記のGitHub上のリポジトリにて公開しています（もちろん、プルリクや実装に関するコメントなどは、受け付けております）.

 * @<href>{https://github.com/tube-shaped-fish-paste-cake/jirou}

手元に持ってきて実際に動作させることは簡単ですので、試してみて欲しいと思います.
プログラムを手元にcloneしたり、実際に動かす方法については、@<chap>{api}で解説していますので、詳しくはそちらをご覧ください.


=== Go is GOD

さて、jirouの実装には、プログラム言語にGoと、KVS（Key Value Store）実装の一つであるLevelDBを使ってみます.
あまり深い理由はなく、なんとなくGoで多少まともな形のコードを書いてみたかったのと（ほら、なんとなくGoっていうと、サーバ書く言語みたいになってるところとかもあるじゃないですか）、
なんとなくKVSというものを使ってみたかっただけです.
要は楽しみつつGoとかKVSの勉強みたいなことをしたいということですね.
手段と目的が逆になってるような感じですが、まあ深くは気にしないで欲しいください.


=== Go 1.5への対応について（するとはいっていない）

本書の執筆中、Go 1.5がリリースされましたが、本書においてはバージョン1.4を前提としています.
1.5での動作確認は行っていませんので、あしからずご了承いただければと思います.

== ソースコードのレイアウト

ソースコードは@<list>{source_tree}のようにレイアウトされています.
エントリポイントが含まれるjirou.goと、パッケージとなるjirouディレクトリとそこに含まれる各種ファイルが主なソースコードになります.
その他、開発の便利のための簡単なシェルスクリプトが含まれます.

//list[source_tree][ソースコードのレイアウト]{
.
├── Gomfile
├── LICENSE
├── README.md
├── curl.sh
├── init.sh
├── jirou
│   ├── help.go
│   ├── option.go
│   ├── server.go
│   └── setup.go
├── jirou.go
└── server.sh
//}

プログラムを構成する主なファイルとその役割については@<table>{table_source}のようになります.

//table[table_source][ソースコードと役割]{
ファイル					役割
-------------------------------------------------------------------------------
jirou.go					エントリポイント
jirou/help.go			ヘルプ表示関数
jirou/option.go		コマンドラインオプションの処理
jirou/server.go		HTTPサーバと具体的なAPIの実装
jirou/setup.go		アプリケーションの初期化処理
//}


== コマンドライン引数の処理とオプションの実装

コマンドラインオプションはjirou/option.goで処理されており、@<list>{option_go}のように処理しています.
サブコマンドなども含まず、あまり複雑でない（というか、そもそもオプションの有無が本の趣旨とあまり関連がない）ことと、
標準のパッケージをなるべく叩いてみようということで、flagパッケージを利用しています.

//list[option_go][option.go オプションの処理]{
package jirou

import "flag"

// Represents command line option
type Option struct {
  Help  bool
  Setup bool
  Port  int
}

func ParseOption() (option *Option) {
  var o = new(Option)
  flag.BoolVar(&o.Help, "help", false, "print help message")
  flag.BoolVar(&o.Setup,
      "setup", false, "insert records to database from file")
  flag.IntVar(&o.Port, "port", 8080, "set port number")
  flag.Parse()
  return o
}
//}

ここでのflagライブラリの使い方はごく単純で、各種のオプションを定義する関数を必要に応じて呼び出す形になっています.
オプションの値を受け取る変数の参照を渡すことで、変数にオプションを処理した結果に応じた値が格納されます.
オプションを定義した後にParse()メソッドを呼び出すことで、コマンドライン引数が実際に処理されます.

各オプションの細かい説明については、@<hd>{api|command_line_option}にて解説しています.
詳しくはそちらをごらん下さい.


== データベースまわりの実装

本書で計画する Web API には各クライアントが厳密に整合性をとるべきデータというのが存在しません.
これはすなわちトランザクションというものを意識しなくても良いということでもあります.

Goにはちょうどいい具合に、シンプルなKVSのライブラリがいくつかあるようです.
今回は@<href>{https://github.com/syndtr/goleveldb, syndtr/goleveldb}@<fn>{fn_goleveldb}というものを使ってみます.
@<href>{http://leveldb.org/, LevelDB}@<fn>{fn_leveldb}がGoogle由来の技術（？）であり、Goと合わせてGoogleで染めていくスタイルでいきましょう.


//footnote[fn_leveldb][http://leveldb.org/]
//footnote[fn_goleveldb][https://github.com/syndtr/goleveldb]


=== データベースの初期化

jirouでは、店舗データを返すAPI用にデータベースを初期化し、店舗データを入れ込む処理をもっています.
このデータはソースコードにJSON文字列リテラルとして記述されています.
データベースの初期化を行う処理を一部抜粋したものが、@<list>{setup}になります.

//list[setup][初期設定の処理]{
func Setup() error {
  option := opt.Options{
    ErrorIfMissing: false,
  }

  var jsonBlob = []byte(`[
    {
      "id" : 0,
      "shop_name" : "ラーメン二郎 三田本店",
      ... 省略
    }
  ]`)

  var blob []interface{}
  err := json.Unmarshal(jsonBlob, &blob)
  if err != nil {
    fmt.Println(err)
    return err
  }

  db, err := leveldb.OpenFile("./jirou.db", &option)
  if err != nil {
    fmt.Println(err)
    return err
  }
  defer db.Close()

  // データベースにデータを保存する
  num := 0
  for _, c := range blob {
    v, _ := json.Marshal(c)
    key := []byte(fmt.Sprintf("%d", num))
    _ = db.Put(key, v, nil)
    num++
  }

  return nil
}
//}

もしかしたら、いちいち大きい文字列で書いた後にバラさず、店舗ごとに一つのJSON文字列リテラルを書いて、
それをPutする処理をずらずら書いた方が良かったかもしれません.
この書き方だと、一部の店舗データを無効にするといったこともやりづらい（コメントアウトなどができない）ですし、
なにより、実装と関係ないところで関数が無駄に長くなっています.


== HTTPサーバ機能の実装

Goの標準ライブラリには、プログラムをHTTPサーバとして動作させるためのパッケージが含まれています.
HTTPサーバに関しては、これをそのまま使ってみたいと思います.

サーバとしてHTTPリクエストの受付を開始する処理は、Server.Run()メソッド内に含まれています（@<list>{start_serving}）.
ポート番号とルーターのインスタンスを引数として、http.ListenAndServe()をコールするだけです.
じつに簡単ですねえ.


//list[start_serving][HTTPサーバの処理を開始する]{
func (self *Server) Run(serverOption *Option) {
  ...
		// Start serving
		fmt.Println("Starting API server, ", serverOption)
		var port string = fmt.Sprintf(":%d", serverOption.Port)
		log.Fatal(
			http.ListenAndServe(port, router))
  ...
}
//}


=== ルーティング

@<hd>{HTTPサーバ機能の実装}にて、ルータという単語が出てきました.
ルータとはURLとサーバプログラム内にあるメソッドをマッピングするために用いるもの、その役割を担うものを指します.
たとえば、example.com/path/to/hoge というURIについてリクエストがあったら、プログラム内のPathToHoge()という処理をコールするとしましょう.
このようなマッピングを行う処理のことをルーティング、それを行うプログラムをルータといいます.

Goには標準的なルータの実装は無く、各種Web系のフレームワークに含まれていたり、そこからフォークして作られたライブラリを使うのが一般的なようです.
今回は@<href>{https://github.com/julienschmidt/httprouter, julienschmidt/httprouter}@<fn>{fn_httprouter}というライブラリを使ってみることにします.

//footnote[fn_httprouter][https://github.com/julienschmidt/httprouter]



httprouter.HttpRouterにはGET()やPOST()といった各種のリクエストメソッドに対応したメソッドが生えています.
これらのメソッドを、パスを表す文字列と、それに対応する関数を引数として呼び出します.
この関数は引数、戻り値といったシグニチャさえ合致していれば構造体のメソッドを指定することも可能で、
レシーバごと渡すとそのまま呼び出されるのがなにげに便利です（小並感）.

実際にルーティングを行っているソースコードを部分的に抜粋したものが@<list>{routing}となります.
self.Search, self.Readなどとしているところに注目してみましょう.
このように記述するだけで、実際にSearchなどが呼び出されるときにちゃんとselfが適当なインスタンスを参照してくれます.

//list[routing][ルーティングを行うソースコードの例]{
// Server class
type Server struct {
  Router   *httprouter.Router
  Db       *leveldb.DB
  DbReport *leveldb.DB
}

func (self *Server) Search(writer http.ResponseWriter,
    request *http.Request, _ httprouter.Params) {
  ...
}

func (self *Server) Run(serverOption *Option) {
  // Build Routing
  router := httprouter.New()
  router.GET("/v1/jirou", self.Search)
  router.GET("/v1/jirou/:id", self.Read)
  router.GET("/v1/jirou/:id/report", self.SearchReport)
  router.POST("/v1/jirou/:id/report", self.PostReport)
  ...
}
//}

@<code>{router.GET("/v1/jirou", self.Search)}などはわかりやすい例でしょう.
これは見たまんま、/v1/jirouというパスでアクセスがきたら、self.Search()を呼び出すものです.

@<code>{router.GET("/v1/jirou/:id", self.Read)}は、
コールバックされるメソッドの処理の中で、URIの一部を引数のように扱うためのものです.
たとえば、/v1/jirou/1などといったパスにアクセスした場合、コールバックではidという名前で1という値が取得できます.
より具体的な例を@<hd>{header_server_read}にて解説しています.


== APIメソッドの実装例

=== 例1 Server.Search()

GoのプログラムをHTTPサーバとして動作させ、ルーティングを行うところを解説しました.
続いて、実際にAPIの呼び出しによって行われる処理とその実装について紹介します.
サンプルとして、Server.Search()メソッドをとりあげます（@<list>{server_search}）.


//list[server_search][Server.Search()]{
func (self *Server) Search(writer http.ResponseWriter,
    request *http.Request, _ httprouter.Params) {

  values := make([]string, 0, 5)
  iter := self.Db.NewIterator(nil, nil)
  for iter.Next() {
    values = append(values, string(iter.Value()))
  }
  iter.Release()
  err := iter.Error()
  if nil != err {
    // Error
  }

  content := strings.Join(values, ",")
  response := []byte(fmt.Sprintf(`
    {
      "content" : [ %v ]
    }`,
    content,
  ))

  writer.Header().Set("Content-Type", "application/json")
  writer.Write(response)
}
//}

HTTPリクエストに応じてコールされてくるこの関数は、引数にwriter http.ResponseWriterというのを持っています.
このwriterに生えているHeader()やWrite()といったメソッドが実際にレスポンスの内容を設定します.
それより先にごにょごにょと処理をしていますが、これらはDBへのアクセスを行ったり、レスポンスとなる文字列を組み立てる、
といった内容になっています.

==={header_server_read} 例2 Server.Read()

もうひとつ、URIの一部を引数のように扱うためのhttprouter.Paramsを使った例として、
Server.Read()の実装について紹介しましょう（@<list>{server_read}）.

//list[server_read][Server.Read()]{
func (self *Server) Read(writer http.ResponseWriter, request *http.Request, params httprouter.Params) {

	key := []byte(params.ByName("id"))
	content, _ := self.Db.Get(key, nil)

	response := []byte(fmt.Sprintf(`
		{
			"content" : %v
		}`,
		string(content),
	))

	writer.Header().Set("Content-Type", "application/json")
	writer.Write(response)
}
//}

このコードの下記の二行で、URIの一部を引数のように取得するための処理を行っています.
ルーティングの設定（@<list>{routing}）でURIパターン文字列に:idというものを指定したのを思い出してみましょう.
この:idというのを、セミコロンを省いた形を名前として、httprouter.Params.ByName()というメソッドで取り出すことができます.

//list[server_read_params][Server.Read()]{
key := []byte(params.ByName("id"))
content, _ := self.Db.Get(key, nil)
//}
