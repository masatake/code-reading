1. 順方向のフロー追跡(1)
------------------------------------------------------------------------

1. /usr/bin/testコマンドについてmanページを読み以下の演算子の意味を調べよ。

   * `=`
   * `!=`
   * `-n`
   * `-z`
   * `-lt`
   * `-ge`

2. 各演算子について具体的などのような「テスト」を実施しているのか調べよ。
   
   - どこから読み始めたか(ベースポイント)
   - どの行から結論を得たか(プルーフポイント)

2. 順方向のフロー追跡(2)
------------------------------------------------------------------------
* mkstempの定義を調べ、ファイルを作成している箇所までのコードパスを特定せよ。
  (mkstempはglibcに定義されている。)
* 「第三引数にO_CREATをつけて __open関数を呼び出している」をもって
   ファイルを作成している箇所とする。
* 力が余っている人は__open関数の定義を調べても良い。


3. 逆方向のフロー追跡(1)
------------------------------------------------------------------------
* touchコマンドを使うと空のファイルを作ることができる。
* 一般ユーザで / 以下にファイルを作成しようとすると失敗する。
  

.. code-block:: console

	$ LANG=C touch /a
	touch: cannot touch `/a': Permission denied

* ソースコードを読んでメッセージが出る経緯を説明せよ。
* (メッセージが翻訳されるのを抑制するためのLANG=Cを指定している。)

4. 逆方向のフロー追跡(2)
------------------------------------------------------------------------
loggerコマンドを使うとコマンドライン経由で/var/log/messagesにログを残せる。

.. code-block:: console

   $ /usr/bin/logger -t name-of-this-program "EXAMPLE"
   # tail -1 /var/log/messages
   Nov  7 02:57:12 localhost name-of-this-program: EXAMPLE

1. loggerコマンドのソースコードを特定せよ。
2. ログの書き込み依頼にwriteシステムコールを使っている。場所を特定せよ。
3. ログ中の時刻をどのように取得しているか、writeシステムコールの箇所から
   逆方向にフローをたどり調べよ。


5. Makefileの読解(1)
------------------------------------------------------------------------
ホスト間のネットワークの疎通を確認するツールにpingがある。pingコマンドのビルド
に使われるMakefileを調べ、pingコマンドのソースコードを特定せよ。

6. Makefileの読解(2)
------------------------------------------------------------------------

1. ftpパッケージのソースコードを入手して、~/fedoraディレクトリに配置せよ。
2. Makefileを調べてftpコマンドを構成するソースコードファイルを特定せよ。

7. Makefileの読解(3)
------------------------------------------------------------------------
「演習1: ソースコードの入手」で universal-ctags を ~/upstreamディレクトリ
にダウンロードしているはずです。

まずこれをビルドしてみましょう。環境によっては、make パッケージと
autoconfパッケージが追加で必要になるかもしれません。

.. code-block:: console

	$ cd ~/upstram/ctags
	$ autoreconf -f -i -v
	$ ./configure
	$ make

順調に行けば、~/upstram/ctags という実行ファイルが生成されるはずです。

.. code-block:: console

    $ ./ctags --version
    Universal Ctags Development(7b40b46), Copyright (C) 2015 Universal Ctags Team
    Universal Ctags is derived from Exuberant Ctags.
    Exuberant Ctags 5.8, Copyright (C) 1996-2009 Darren Hiebert
      Compiled: Oct 29 2015, 20:52:17
      URL: https://ctags.io/
      Optional compiled features: +wildcards, +regex, +debug, +option-directory, +coproc


さて、この出力中 7b40b46 (環境によっては別の文字列かもしれません) はどこから
やってきたか、ビルドプロセスを調べて コードパスを示して下さい。

8. straceの活用
------------------------------------------------------------------------

1. straceの元で/usr/bin/pwdコマンドを実行せよ。
2. 出力を見て「核心」と考えらるシステムコールを探せ。
3. pwdのソースコードで、そのシステムコールを呼び出している箇所を探せ。

9. ソースコードの切り出し
------------------------------------------------------------------------
~/released/sendmail-8.14.6/sendmail/convtime.cにconvtime関数が定義
されている。この関数は第一引数に数値を、第二引数に単位を指定して、結果
を秒で返す。

この関数単体の動作を調べるために、コマンドラインから引数を与えて実行でき
るようにしたい。関数を抜き出して、main関数を加えたソースコードを作成せよ。

想定する実行例

.. code-block:: console

    $ ./a.out 1 s
    1
    $ ./a.out 1 m
    60
    $ ./a.out 1 h
    3600
    $ ./a.out 1 d
    86400
    $ ./a.out 2 h
    7200

想定するmain関数(エラーチェックはしない)

.. code-block:: c

    int
    main(int argc, char** argv)
    {
      char *P;
      int UNITS;

      P = argv[1];
      UNITS = argv[2][0];

      printf("%ld\n", convtime(P, UNITS));

      return 0;
    }

使って良いハック

.. code-block:: c

    #define sm_strcasecmp strcasecmp
    #define usrerr(...) { fprintf (stderr, __VA_ARGS__); exit(1); }

.. 以下のようにして、一般で/rootにcdすると失敗する。

..     [yamato@localhost ~]$ cd /root
..     bash: cd: /root: Permission denied
..     [yamato@localhost ~]$ exit


   
.. * /srv/sources/sources/b/bash/4.1.2-9.el6_2/pre-build/bash-4.1/test.c
.. * 次のプログラムをコンパイルしてstraceの元で実行し、どのような
..   システムコールが起動されているか調べよ。

.. .. code-block:: c
..   :linenos:

..    int main(void) { return 0; }

   
.. * straceはどのような仕掛けで引数で指定したプログラムの
..   システムコールの起動を追跡しているか、straceで調べよ。

.. * システムコールの一覧とerrnoの取り得る値の一覧を見よ。

..      - /usr/include/asm-generic/errno-base.h
..      - /usr/include/asm-generic/errno.h
..      - /usr/include/asm-generic/unistd.h  
..      - /usr/include/asm/unistd_64.h

.. * strerror関数を用いて、エラー番号とその説明の一覧を表示するプログラムを
..   作成せよ。

.. * xxxというコマンドについてダイナミックにリンクされているライブラリ
..   を列挙し、さらにそのライブラリが所属するパッケージ名を挙げよ。


.. .. TODO: ログ増強の方法

