= あとがき

== それがセクシー・・・

本書をお手にとっていただき、ありがとうございます。
今回はどういったネタでいこうかなかなか決まらず、スケジュールがセクシーな感じになってしまいました。
そのせいで、全体的に内容をコンパクトにまとめる方向にしなくてはなってしまいました。
Goについてもまだまだ勉強中で、今回のネタに合わせて実装を作るのにも手間取りました。


== Goについて思うこと

Goに触れて思ったのは「イデオミックでベターなCっぽい」という事でした。
C言語のようなプログラミング言語はありましたが、それらのほとんどは実は表面的なもので、
C言語の後ろに立った言語で、ここまで真後ろに立ったものは無かったのではないか、という風にも思います。


たとえば関数におけるエラーの伝達は戻り値を基本として行うところなんかどうでしょうか。
Goでは、これを言語の機能で支えるため、いくつかの戻り値をとる関数を記述できるようになっていて、エラーがある場合はそれを返す、という風になっています。

これがC言語となると、戻り値は必ず一つとなるため、それによって正常値とエラー値をシグニチャ上で区別することはできません。
したがって、呼び出す側においてもエラーかそうでないかといった結果を、一つの方法で受け取った上で判断しなければならないということでもあります。
具体的な例として、処理に失敗したらNULLを返す、成功したのであれば何かしらのオブジェクトを指すポインタを返す、という古典的な処理について考えてみましょう
これは、NULLのポインタはエラーですよという風に恣意的な解釈を行っているに過ぎないと言えるのではないでしょうか。

あるいは、C++やJavaを始めとする言語では、これを例外によって解決しようとし、一部は成功し、そして一部は失敗に終わりました。
よりスマートなエラー制御を目指したはずの例外がなんとなく扱いにくいと思われているのは（と思ってるのはぼくだけでしょうか？）、
制御構文のようであり、なおかつ戻り値の内容によってエラーであるか否かとする手法と排他的じゃなかなったせいなんじゃないかと。

#@# 構造体とレシーバの関係なんかも個人的には面白いです。
#@# これは、オブジェクト指向におけるクラスとメソッドであるようでいて、実際はけっこう違うように思います。
#@# 思うに、self的な構造体のポインタを受け取る関数でもってオブジェクト試行的なプログラミングをやるときの、Cにおけるイディオムの再発明ではないかといった具合です。



== そろそろおしまい

だらだら書いていたら高説タレ夫みたいになってしまって具合が悪くなってきました。
まあ、今までにないデザインの言語に触れて、こんな風にアレコレ考えたりするというのはいろんな気付きがありますし、
思いの外楽しいものでもあります。

引き続きGo言語についてはいろいろやっていこうと思っていて、今後もGo言語ネタでドロップしていきたいかなと思っています。
本書を書いている最中に、バージョン1.5がリリースされたり、GoでAndroid向けのアプリが書けるよ、なんてネタも出てきています。
まだまだいろいろ楽しめそうです。

時間も紙幅も余裕がなくなってきました。
そろそろ終いにしたいと思います。
またいつか、コミケでお会いしましょう。

7月某日 @kandayasu


#@warn(メタ情報について追加する)
#@warn(あとがきとスペシャルサンクス)

本書の作成にあたり、@<href>{https://github.com/kmuto/review/, Re:ViEW}を使用しました。
原作者、コミッタ、コントリビュータ各位に感謝致します。
