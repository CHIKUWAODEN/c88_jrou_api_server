= 設計を考える

//read{
まずは設計を考えていきます。
Web アプリケーションの開発を嗜む人から見れば少々退屈な内容かもしれませんが、お付き合いください。
そもそも Web API における設計というと、具体的にどのようなものでしょうか。
本書では Web API は URI を指定して、HTTP を経由してリクエストされるという根本的な点に注目してみようと思います。
//}


== URI（Uniform Resource Identifier）

URI（Uniform Resource Identifier）はその名が表すとおり、インターネット上のリソースの有りかを示すものです。
つまり、Web API とはリソースそのものであると考えることができると思います。


== HTTP（Hyper Text Transfer Protocol）

もう一つ、HTTP@<fn>{http11_rfc} というものについて触れてみます。
HTTP（Hyper Text Transfer Protocol）はインターネット上でサーバとクライアントが文書（データ）をやりとりするためのプロトコルです。

この HTTP の仕組みを簡単に説明するとともに、
このプロトコル上でどのように Web API といったものを作っていくかという視点で進めていきます。

//footnote[http11_rfc][RFC2616 : https://www.ietf.org/rfc/rfc2616.txt]

=== リクエストメソッド

ここでリクエストメソッドという言葉が出てきました。
リクエストメソッドには次のような種類があり、「サーバに対してどのような動機でリクエストを行ったか」を表します。
本書を執筆している時点でもっとも多くされているであろう HTTP 1.1 という仕様では、次のようなリクエストメソッドが定義されています。


//table[http_req_methods][HTTP 1.1 のリクエストメソッド]{
メソッド        意味
-------------------------------------------------------------------------------
GET						  リソースの取得
POST						リソースの新規作成
PUT						  リソースの更新
HEAD						リソースの
DELETE					リソースの削除
OPTION					リソースの
TRACE					  リソースの
CONNECT				  リソースの
//}


=== レスポンス

Web サーバに対してリクエストを行うと、その応答としてレスポンスを返します。
レスポンスには、サーバが返すデータと、そのリクエストに対してどのような応答をしたかを表すステータスコードというものが含まれています。



== あらためて、Web APIの設計について

== 二郎をリソースという概念で表現する

まず二郎のもつ性質について分析しましょう。
一口に二郎といってもさまざまです。まず店舗という単位が存在し、それらには「店名」「住所」といった普遍的なプロパティを持ちます。
ついで「量」であるとか「スープの質」「価格」「メニュー」「席数」といった点からも分類することが可能でしょう。
これらを踏まえた上で、JSON フォーマットでどのように二郎を表現すれば良いのか考えてみます。


//list[jirou_json_format][ラーメン二郎のデータを JSON フォーマットで表現する][json]{
{
  shop_name : "ラーメン二郎 歌舞伎町店",
  open      : "11:30",
  last      : "04:30",
  regular_holiday : []
  address : {
    prefecture  : "東京都",
    city        : "新宿区",
    address1    : "歌舞伎町",
    address2    : "1-19-3",
    postal_code : "160-0021"
  },
  menues : [
    { name : "普通盛"                 , price : 700  },
    { name : "チャーシュー"           , price : 800  },
    { name : "チャーシューダブル"     , price : 900  },
    { name : "大盛"                   , price : 800  },
    { name : "大盛チャーシュー"       , price : 900  },
    { name : "大盛チャーシューダブル" , price : 1000 },
    { name : "つけ麺普通"             , price : 800  },
    { name : "つけ麺大盛"             , price : 900  },
    { name : "キムチ"                 , price : 200  },
    { name : "煮玉子"                 , price : 100  }
  ],
  created   : "Fri Jun 12 23:03:50 JST 2015",
  modified  : "Fri Jun 12 23:03:50 JST 2015",
}
//}

ラーメン二郎歌舞伎町店の情報を参考に、大体このような感じでまとめてみました。
それぞれのキーの持つ意味については、次のようになります。


//table[key_description][API の戻り値に含まれる各キーの説明]{
キー									データ型					説明
-------------------------------------------------------------------------------
shop_name							String						店舗名
open									String						オフィシャル開店時間
last									String						オフィシャル閉店時間
regular_holiday				Array of String		定休日
address								Object						住所
address.prefecture		String						都道府県
address.city					String						市区町村
address.address1			String						住所1
address.address2			String						住所2
address.postal_code		String						郵便番号
menues								Array of Object 	メニュー
menues[].name					String						商品名
menues[].price				Integer						値段
created								String						データ登録日（文字列表現）
modified							String						データ更新日（文字列表現）
//}
