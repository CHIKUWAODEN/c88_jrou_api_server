= 設計を考える

//read{
まずは設計を考えていきます.
同人誌のネタなので、あまり派手なことは考えません.
今回はフロントエンドは省いてWeb APIを提供するサーバサイドのプログラムのみを作成します.
//}


== なにをするか決める

まずはどういったことをするのか、APIが表現するものはなにかといったことを決めます.
今回はラーメン二郎の情報を扱うということで、次のような要件を立ててみました.

 * 各店舗の所在地や営業時間などのプロフィールが取得できる
 * その日のブレ具合などをレポートとして送信したり、それを一覧として取得できる

やることもシンプルなので、APIもシンプルにまとめます.
店舗情報の更新・削除系のAPIを持たないのは、そういった情報を頻繁に更新するケースが想像しづらかったため、
簡単のために省いています.

//table[api][API一覧]{
URI														メソッド		説明
-------------------------------------------------------------------------------
/jirou/v1/jirou								GET					二郎前店舗の情報を取得する
/jirou/v1/jirou/:id						GET					IDを指定し、ある店舗の情報を取得する
/jirou/v1/jirou/:id/report		GET					IDを指定し、ある店舗のレポートを取得する
/jirou/v1/jirou/:id/report		POST				IDを指定し、ある店舗のレポートを送信する
//}


== 各APIのもう少し詳しい設計

=== GET /jirou/v1/jirou

まず二郎のもつ性質について分析しましょう.
一口に二郎といってもさまざまです.まず店舗という単位が存在し、それらには「店名」「住所」といった共通の属性を持ちます.
ついで「量」であるとか「スープの質」「価格」「メニュー」といった点も、店舗ごとの情報としては必要そうです.
これらをふまえた上で、どのようなデータ構造にするのか考えてみます.

//table[key_description][API の戻り値に含まれる各キーの説明]{
キー									データ型					説明
-------------------------------------------------------------------------------
id											int								店舗ID
shop_name								string						店舗名
business_hours					array of object		開店時間
business_hours[].open		string						オフィシャル開店時間
business_hours[].last		string						オフィシャル閉店時間
regular_holiday					array of string		定休日
address									object						住所
address.prefecture			string						都道府県
address.city						string						市区町村
address.address1				string						住所1
address.address2				string						住所2
address.postal_code			string						郵便番号
menues									array of object 	メニュー
menues[].name						string						商品名
menues[].price					integer						値段
//}

regular_holiday は sun, mon, tue, wed, thu, fry, sat, national のいずれかを取ります.
sun から sat まではそれぞれウィークデーを表し、national は祝祭日などを表すものとします.

実際に GET /jirou/v1/jirou が返すデータは次のようになります.

//list[jirou_json_format][GET /jirou/v1/jirou]{
{
  "content": [
    {
      "address": {
        "address1": "三田",
        "address2": "2-16-4",
        "city": "港区",
        "postal_code": "108-0073",
        "prefecture": "東京都"
      },
      "business_hours": [
        {
          "last": "15:30",
          "open": "08:30"
        },
        {
          "last": "20:00",
          "open": "17:00"
        }
      ],
      "created": "Fri Jun 12 23:03:50 JST 2015",
      "id": 0,
      "menues": [
        {
          "name": "ラーメン",
          "price": 600
        },
        {
          "name": "ぶたラーメン",
          "price": 700
        },
        {
          "name": "ぶたダブルラーメン",
          "price": 800
        },
        {
          "name": "大ラーメン",
          "price": 650
        },
        {
          "name": "ぶた入り大ラーメン",
          "price": 750
        },
        {
          "name": "ぶたダブル大ラーメン",
          "price": 1000
        }
      ],
      "modified": "Fri Jun 12 23:03:50 JST 2015",
      "regular_holiday": [
        "sun",
        "national"
      ],
      "shop_name": "ラーメン二郎 三田本店"
    },
    {...},
    {...},
    {...},
  ]
}
//}




=== GET /jirou/v1/jirou/:id

これは単にIDを指定して、ある一つの店舗データを返してもらうものであり、根本的には@<table>{key_description}で示したものと同じフォーマットで返します.
/jirou/v1/jirou の場合、複数の値を配列として返してたところ、
こちらのAPIは必ず一店舗のみのデータを返すため、ある一つのオブジェクトを表すような形にだけなっています.


=== POST /jirou/v1/jirou/:id/report

問題となるのは、味わいやクセといった、その評価が個人の主観に依存する部分です.
具体的には、ブタ（チャーシュー）やスープの仕上がりといったものです.
味覚について計測する標準的な手法は存在しないため、これらは個々人の感覚に頼った評価をしなくてはならないという点が問題になります.

ぐるなびなどを例にすると分かりやすいのですが、こういったものはレート制を導入するのがよくあるパターンでしょう.
主観的となる評価を一定の範囲内で点数として表してもらい、それを平均するなどした値を、味に対するユーザー全体の意見として利用します.

//list[report_api_post_data][レポートAPIに送るJSONデータのサンプル]{
{
  "reporter" : "Jhon Smith",
  "comment" : "lorem ipsum dolor sit amet ... (Free comment)",
  "rating" : {
    "noodle" : 5.0,
    "soup" : 5.0,
    "poke" : 5.0
  }
}
//}

#@warn(戻り値についてとそのサンプルを書く)
#@warn(内部的にcreatedやUUIDなどの情報を追加していることを書く)


=== GET /jirou/v1/jirou/:id/report  

この API はある店舗のレポートの一覧を返します.
基本的には POST /jirou/v1/jirou/:id/report で投稿したデータと同じものが返ってくるのですが、
内部で生成日時（レポート日時）であるとか UUID などの情報を付け加えて保存しているため、
@<list>{report_api_get_format} のような形で返されてきます.


//list[report_api_get_format][GET /jirou/v1/jirou/:id/report の出力サンプル]{
{
  "content": [
    {
      "reporter": "Jhon Smith",
      "comment": "lorem ipsum dolor sit amet ... ",
      "rating": {
        "noodle": 5,
        "soup": 5
      },
      "date": "Mon, 20 Jul 2015 00:50:36 +0900",
      "uuid": "058657f6-6ca9-49ee-890d-7d0b221462b7",
      "shop_id": "4"
    },
    { ... },
    { ... },
    { ... }, ...
  ]
}


//}

#@# == API エラーの設計
#@# 
#@# エラーをどのように返すのが良いか.
#@# HTTP のレスポンスコードと、それに含まれるコンテンツについて考えます.
