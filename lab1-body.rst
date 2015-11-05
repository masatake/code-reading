* 端末を起動しましょう。
* エディタを準備しましょう。
* ソースコードを入手しましょう。
* ソースツリーを探索しましょう。

端末の起動
------------------------------------------------------------------------

.. kdeではkonsole, kwriteが利用できる。

1. 画面左下のメニューからApplicatoinsを選ぶ。
2. メニューアイテムSystemからTerminal(konsole)を選ぶ。

エディタの準備
------------------------------------------------------------------------
好みのエディタがあれば、ここでインストールして下さい。
特に好みが無ければkwriteというKDEデスクトップに付属するエディタが
利用できます。

システムの設定を変更するのに管理者権限
でエディタを動かす必要があります。
次のようにsuコマンドを使います。
(suの後のハイフンを忘れないで下さい。)

.. code-block:: console

	$ su -
	Password: 
	# kwrite 

ツールのインストール
------------------------------------------------------------------------
.. code-block:: console

        $ su
	Password: 
	# yum -y install git rpm-build firefox gcc subversion \
	         man-pages wget ctags autoconf make strace
	...
	Is this ok [y/N]: y
	...
	# exit

ソースコード配置用ディレクトリの作成
------------------------------------------------------------------------
.. code-block:: console

	$ cd 
	$ mkdir upstream
	$ mkdir released
	$ mkdir fedora


アップストリームプロジェクトより入手(1)
------------------------------------------------------------------------
helloの最新のコードを開発元リポジトリより入手します。

.. code-block:: console

	$ cd ~/upstream
  	$ git clone git://git.savannah.gnu.org/hello.git
  	$ cd hello
  	$ git submodule init
  	$ git submodule update

アップストリームプロジェクトより入手(2)
------------------------------------------------------------------------
universal ctagsの最新のコードを開発元リポジトリより入手します。

.. code-block:: console

	$ cd ~/upstream
	$ git clone https://github.com/universal-ctags/ctags

ctagsは古くから存在するソフトウェアですが、最近(2012,3年頃)から開発が
停滞していました。筆者らは、フォークしたプロジェクト(universal ctags)
にて開発を続けています。

アップストリームプロジェクトより入手(3)
------------------------------------------------------------------------
universal ctagsの最新のコードを開発元リポジトリより入手します。

.. code-block:: console

	$ cd ~/upstream
	$ git clone https://github.com/universal-ctags/ctags

* ctagsは古くから存在するソフトウェアですが、最近(2012,3年頃)から開発が
  停滞していました。

* 筆者らは、フォークしたプロジェクト(universal ctags)にて開発を続けています。

アップストリームプロジェクトより入手(4)
------------------------------------------------------------------------
coreutilsの最新のコードを開発元リポジトリより入手します。

.. code-block:: console

	$ rpm -qi coreutils

の出力を参照して、coreutilsの最新のソースコードを入手して
~/upstream以下に配置して下さい。

アップストリームプロジェクトより入手(4)
------------------------------------------------------------------------
sendmailの最新のリリース版をダウンロードして、展開の後 ~/releasedに
配置して下さい。

.. code-block:: console

	$ wget ftp://ftp.sendmail.org/pub/sendmail/sendmail.8.14.6.tar.gz
	$ tar zxvf sendmail.8.14.6.tar.gz
        $ mv sendmail-8.14.6 ~/released

ソースパッケージより入手: ソースパッケージリポジトリの設定変更
------------------------------------------------------------------------
glibcとcoreutilsのソースコードのソースパッケージ経由で
入手しましょう。

/etc/yum.repos.d/fedora.repo及びfedora-updates.repoの
[fedora-source]及び[updates-source]セクション
にあるenableのフィールドを0から1に変更して保存します::

    [fedora-source]
    name=Fedora $releasever - Source
    ...
    enabled=1
    ...

    [updates-source]
    name=Fedora $releasever - Updates Source
    ...
    enabled=1
    ...

ソースパッケージより入手: ダウンロード
------------------------------------------------------------------------
coreutilsとglibcのソースパッケージをダウンロードします。

.. code-block:: console

	$ cd /tmp
	$ yumdownloader --source coreutils

	
ソースパッケージより入手: 依存パッケージのインストール
------------------------------------------------------------------------
ソースコードツリーを合成するのに必要となるパッケージを
インストールします。

.. code-block:: console

	$ su
	Password:
	# cd /tmp
	# yum-builddep coreutils*.src.rpm
	...
	Is this ok [y/N]: y
	...
	# exit

ソースパッケージより入手: ソースパッケージのインストール
------------------------------------------------------------------------
入手したsrc.rpmを展開します。 

.. code-block:: console

	$ cd /tmp
	$ rpm -ivh coreutils-*.src.rpm

~/rpmbuild以下に展開されます。
	
	
ソースパッケージより入手: 合成
------------------------------------------------------------------------
specファイルを指定してソースコードツリーを合成します。

.. code-block:: console

	$ cd ~/rpmbuild
	$ rpmbuild -bp SPECS/coreutils.spec

ソースコードツリーが~/rpmbuild/BUILD/coreutils-8.15に配置
されます。(環境によってはバージョン番号部分は異なるかも
しれません。)

ソースパッケージより入手: 配置
------------------------------------------------------------------------
合成したソースコードツリーを~/fedoraへ移動させます。

.. code-block:: console

	$ mv ~/rpmbuild/BUILD/coreutils-8.15 ~/fedora


ソースパッケージより入手: 課題
------------------------------------------------------------------------
glibcについてもソースパッケージを入手して、展開、合成、配置まで
のステップを実施して下さい。


ソースコードツリーの観察
------------------------------------------------------------------------
* ソースコードツリーを探索して
  どのようなファイルがどのディレクトリに含まれているか調べて
  下さい。

    - ~/upstream/hello
    - ~/upstream/coreutils
    - ~/released/sendmail-8.14.6
    - ~/fedora/coreutils-*
    - ~/fedora/glibc-*

* upstreamのcoreutilsとfedoraのcoreutilsの2つのソースコード
  ツリーにどのような違いがあるか調べて下さい。

関数を読んでみる(1)
------------------------------------------------------------------------
以下の関数の定義を行単位で説明して下さい。

* fedora/glibc-2.15-a316c1f/string/strcmp.cに定義された
  strcmp関数

* fedora/glibc-2.15-a316c1f/string/strcasecmp.cに定義された
  strcasecmp関数

* fedora/glibc-2.15-a316c1f/string/strcat.cに定義された
  strcat関数

* fedora/glibc-2.15-a316c1f/string/strdup.cに定義された
  strdup関数

関数を読んでみる(2)
------------------------------------------------------------------------
~/released/sendmail-8.14.6/sendmail/convtime.cに2つの関数が定義
されています。

* convtime
* pintvl

各関数の冒頭には関数の役割が記載されています。記載された通り実装されて
いるか関数定義を読んで確認して下さい。

関数を読んでみる(3)
------------------------------------------------------------------------
1. C言語文字列の長さを返す関数strlenを独自に実装して下さい。
2. ~/fedora/glibc-2.15-a316c1f/string/strlen.cに定義されたstrlen関数
   と比較してみて下さい。
3. glibc中のstrlen関数の定義を説明して下さい(*)。


