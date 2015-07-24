= プログラムとして動かしてみる

//read{
実装したプログラムを実際に動作させてみます。
まえがきでも触れましたが、ソースコードはすべて GitHub で公開しています。
なので、少しだけ環境を整える手間をかければ、お手元で動作させていただくことが可能です。
Git といったツールや Github については本誌では解説はしていませんが、あしからず了承ください。
//}


== セットアップ


=== 補助的なツールのインストール

開発環境は、次にあげる各種ツールに依存しています。
筆者はOS X上で作業しており、コマンドラインなどはこれを前提とした格好になっていますが、
LinuxなどのOSであれば、同様のツールチェインをそろえるのは容易かと思います。
Windows上での動作はまったく考慮していないため、もし手元にWindowsしか無いような場合は、
VirtualBoxなどで仮想化環境を用意するほうが、手っ取り早いかと思います。

 * @<href>{http://curl.haxx.se/, curl} 説明不要かと
 * @<href>{http://stedolan.github.io/jq/, jq} コマンドラインJSONプロセッサ
 * @<href>{https://github.com/mattn/gom, gom} パッケージマネジメントツール

//list[][]{
$ brew install jq curl
$ go get github.com/mattn/gom
//}


=== コードのクローン

実際にプログラムを動かしてみましょう。
コードは@<href>{https://github.com/tube-shaped-fish-paste-cake/jirou}として公開しています。
次のようなコマンドでクローンすることができます。

//list[][]{
$ git clone https://github.com/tube-shaped-fish-paste-cake/jirou.git
//}


== APIサーバの初期化手続き

店舗情報などのデータを予めサーバが使えるような形にセットアップするための処理を行います。
このプログラムは、先に述べたとおり --setup オプションによって起動することができます。
gom によってパッケージ管理をおこなっているため、gom run としている点に注意しましょう。

//list[][]{
rm -rf ./jirou.db
gom run jirou.go --setup
//}


== プログラムを起動する

シンプルに go run（gom run）コマンドによってメインプログラムから起動します。
仮想化環境上で動かすとか、Dockerとか、インターネット経由で見てもらえるように
ちゃんと公開できるようにしようかとか、いろいろ考えましたが、シンプルに寄せようという結論になりました。

//list[][]{
$ gom run jirou.go
Starting API server,  &{false false 8080}  
//}

問題なくサーバが起動できれば、Starting API server,  &{false false 8080} という風に表示され、
リクエストを待つような状態になり、localhost:8080としてアクセスできるようになります。
続いて、これのサーバにアクセスしてみましょう。


== curlによるAPIへのアクセス

ここではAPIを利用するシンプルな例として、curlコマンドを利用して API にアクセスしてみようと思います。
Unix系のOS であれば、ほぼ問題なく利用できるでしょうし、スクリプトとして表現しやすい（コードとしてリポジトリに入れやすい）です。
また、フロントエンド（クライアントサイド）のアプリケーションを書くのは、紙幅と時間の都合でつらかったというのもあります。


//list[][APIへアクセスするcurlコマンドの例]{
$ curl -s \
  -X GET \
  -H "Content-Type: application/json" \
  "localhost:8080/v1/jirou" | jq "."
//}


curlはファイルへの保存オプションを利用しない場合、レスポンスの内容を標準出力に送ります。
-sオプションと併用することで、APIが返すJSON文字列をそのまま標準出力にお送ることができます。
上記の例では、整形と、JSONとして正しい文字列になっているかの簡単な検査のために、
パイプで jq コマンドに送っています。


//list[][POSTでデータを送信する]{
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
