={run_program} プログラムとして動かしてみる

//read{
実装したプログラムを実際に動作させてみます.
まえがきでも触れましたが、ソースコードはすべて GitHub で公開しています.
なので、少しだけ環境を整える手間をかければ、お手元で動作させていただくことが可能です.
Git といったツールや Github については本誌では解説はしていませんが、あしからず了承ください.
//}


=={setup} セットアップ

=== 補助的なツールのインストール

開発環境やテスト環境は、いくつかの外部ツールを利用しています.
筆者はOS X上で作業しており、コマンドラインなどはこれを前提とした格好になっていますが、
LinuxなどのOSであれば、同様のツールチェインをそろえるのは容易かと思います.
Windows上での動作はまったく考慮していないため、もし手元にWindowsしか無いような場合は、
VirtualBoxなどで仮想化環境を用意するほうが、手っ取り早いかと思います.

: @<href>{http://curl.haxx.se/, curl}
  説明不要かと
: @<href>{http://stedolan.github.io/jq/, jq}
  コマンドラインJSONプロセッサ
: @<href>{https://github.com/mattn/gom, gom}
  Goのパッケージマネジメントツール（RubyでいうところのBundler的なもの）

//list[install_tools][jirouをサポートするツールのインストール]{
$ brew install jq curl
$ go get github.com/mattn/gom
//}


==={clone_from_github} コードのクローン

実際にプログラムを動かしてみましょう.
コードは@<href>{https://github.com/tube-shaped-fish-paste-cake/jirou}として公開しています.
次のようなコマンドでクローンすることができます.

//list[clone_from_github_command][jirouのソースコードをGitHubからcloneする]{
$ cd path/to/work
$ git clone https://github.com/tube-shaped-fish-paste-cake/jirou.git
//}


=={command_line_option} コマンドラインオプションついて

: --help
  ヘルプを表示します.
: --setup
  アプリケーションの動作に必要なデータベースを生成します.
  サーバとして起動する前に一度呼び出すようにします.
: --port
  APIサーバ（HTTPサーバ）として動作するさいのポート番号を指定します.
  オプションを指定しない場合は、8080番ポートがデフォルトで使用されます.


== APIサーバの初期化手続き

店舗情報などのデータを予めサーバが使えるような形にセットアップするための処理を行います.
このプログラムは、先に述べたとおり --setup オプションによって起動することができます.
gom によってパッケージ管理をおこなっているため、gom run としている点に注意しましょう.

//list[jirou_setup][jirouの初期化処理]{
gom run jirou.go --setup
//}

--setupオプションをつけて実行すると、店舗情報などを格納したデータベースやレポートを格納するためのDBが生成されます.
このデータベースは単なるディレクトリとその内部のファイルなので、次のように削除することで消去することができます.

//list[jirou_reset][jirouのデータベースをクリアする]{
rm -rf ./jirou.db
rm -rf ./report.db
//}

== プログラムを起動する

シンプルに go run（gom run）コマンドによってメインプログラムから起動します.
仮想化環境上で動かすとか、Dockerとか、インターネット経由で見てもらえるように
ちゃんと公開できるようにしようかとか、いろいろ考えましたが、シンプルな方に寄せることにしました.

//list[run_jirou][jirou APIサーバを起動する]{
$ gom run jirou.go
Starting API server,  &{false false 8080}  
//}

問題なくサーバが起動できれば、Starting API server,  &{false false 8080} という風に表示され、
リクエストを待つような状態になり、localhost:8080としてアクセスできるようになります.
続いて、これのサーバにアクセスしてみましょう.


== curlによるAPIへのアクセス

ここではAPIを利用するシンプルな例として、curlコマンドを利用してAPIにアクセスしてみようと思います.
Unix系のOSであれば、ほぼ問題なく利用できるでしょうし、スクリプトとして表現しやすい（コードとしてリポジトリに入れやすい）です.
（といった記事をQiitaだったかはてブだったけで最近見かけた気がします）

//list[api_access_example_get][APIへアクセスするcurlコマンドの例]{
$ curl -s \
  -X GET \
  -H "Content-Type: application/json" \
  "localhost:8080/v1/jirou" | jq "."
//}


curlはファイルへの保存オプションを利用しない場合、レスポンスの内容を標準出力に送ります.
-sオプションと併用することで、APIが返すJSON文字列をそのまま標準出力にお送ることができます.
上記の例では、整形と、JSONとして正しい文字列になっているかの簡単な検査のために、
パイプでjqコマンドに送っています.


//list[api_access_example_post][POSTでデータを送信する]{
# Post Report
POST_DATA=`cat <<EOS
{
  "reporter" : "Jhon Smith",
  "comment" : "lorem ipsum dolor sit amet ... (Free comment)",
  "rating" : {
    "noodle" : 5.0,
    "soup" : 5.0
  }
}
EOS
`
for id in {0..4}
do
  echo "POST /v1/jirou/${id}/report"
  echo ${POST_DATA}
  curl -s \
    -X POST \
    -d "${POST_DATA}" \
    "localhost:8080/v1/jirou/${id}/report" | jq "."
done
//}

改行なども含めると長大になる上、中途半端に省略しても意味がなさそうだったので、
出力例については掲載していませんが、よければ試してみてください.
