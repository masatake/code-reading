読解の環境
========================================================================

ソースコードから実行ファイルを構築するのに使う環境(ビルド環境)、実行ファ
イルを実行する環境(実行環境)、演習参加者がソースコードを調査する環境(調
査環境)は全てFedoraとしました。Fedoraを選定したのは筆者がFedoraに技術的
な経験があることと、ソースコードの入手方法が良く整備されているためです。


.. figure:: fedora.svg

FedoraはLinuxカーネルを中心に様々なフリーソフトウェア/オープンソースソ
フトウェア(FOSS)によって構成された、いわゆるGNU/Linuxディストリビューショ
ンの一つです。ディストリビューションにはFedora以外にも様々あります。ど
れも、アップストリームプロジェクト群からソースコードを入手、ビルドして、
簡単にインストールできる形式(パッケージ)に梱包してソフトウェアをユーザー
に提供する、という点でだいたい同じです [#distro]_ 。パッケージの集合体
が1つのシステム全体として機能するように、調整された上で品質検査を経て出
荷されています。ただしディストリビューションによってどういったシステム
を目指しているか、どのようなコマンド体系でパッケージを管理するか、開発
のスケジュールなどが異なります。

パッケージの入手、システムにインストールされたパッケージ情報の入手、
パッケージの構築方法などパッケージに関連するコマンド操作はFedoraは他の
ディストリビューションで異なります。本演習で得た知見を他のディストリビュ
ーションに応用しようとするとまず、対象ディストリビューションのパッケージ
の扱いや操作について知る必要があります。その他の点ではどのディストリビュ
ーションでもそのまま有効なはずです。

他の環境、例えばWindowsやMac OSXとでは違いが大きすぎて、本演習で紹介す
るコマンドラインなどの具体的な手順は役に立たないかもしれません。しかし
コードリーディングの基本的な考え方は同じはずです。ある程度の規模のソフ
トウェアとなるとプラットフォームの違いよりも、プログラミング言語の違い
の方が問題になるでしょう。

.. さらに規模が増すと

.. Fedoraは筆者が勤務するレッドハット社が開発を後援するディストリビューショ
.. ンです。企業ユーズを想定したディストリビューションRed Hat Enterprise
.. Linuxのテストベッドとしての側面を持ちます。

.. [#distro] 実行ファイル、ライブラリをローカルシステムでビルドすること
             を前提にビルド手順の正規化とソースコードの梱包に注力してい
             るディストリビューションもあります。


パッケージ管理
------------------------------------------------------------------------

Fedoraでは様々なソフトウェアをバイナリパッケージ(パッケージ、拡張子rpm)
として簡単にインストールできます。その中には調査に役立つ便利なツー
ルも含まれています。ここでインストールを含むパッケージの活用の方法を紹
介しておきます。

パッケージのインストール
,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,
インストールしたいパッケージの名称がわからない場合、検索できます
::

	$ yum search WORD

とするとWORDに関連するソフトウェアを含むパッケージの名前を列挙できます。
例::

	$ yum search emacs
	読み込んだプラグイン:auto-update-debuginfo, langpacks, presto, priorities, refresh-
			   : packagekit
	============================== N/S Matched: emacs ==============================
	clips-emacs.noarch : EMACS add-ons for the CLIPS expert system
	coq-emacs.noarch : GNU Emacs support for Coq proof management system
	coq-xemacs.noarch : XEmacs support for Coq proof management system
	crm114-emacs.x86_64 : CRM114 mode for Emacs
	cvc3-emacs.noarch : Compiled Emacs mode for CVC3
	cvc3-emacs-el.noarch : Elisp source files for the CVC3 Emacs mode
	cvc3-xemacs.noarch : Compiled XEmacs mode for CVC3
	cvc3-xemacs-el.noarch : Elisp source files for the CVC3 XEmacs mode
	emacs.x86_64 : GNU Emacs text editor
	...

検索結果の中に気になるパッケージがあれば、
::

		$ yum info パッケージ名

として、そのパッケージの詳細を情報を見ることができます。

例::

	$ yum info clips-emacs
	...
	名前                : clips-emacs
	アーキテクチャ      : noarch
	バージョン          : 6.30.0
	リリース            : 0.4.20090722svn.fc17
	容量                : 14 k
	リポジトリー        : fedora
	要約                : EMACS add-ons for the CLIPS expert system
	URL                 : http://clipsrules.sourceforge.net
	ライセンス          : GPLv2
	説明                : This package contains CLIPS emacs scripts.
			    : 
			    : CLIPS (C Language Integrated Production System) is an
			    : expert system development tool which provides a complete
			    : environment for the construction of rule and/or object
			    : based expert systems.
			    : 
			    : Created in 1985 by NASA at the Johnson Space Center, CLIPS
			    : is now widely used throughout the government, industry,
			    : and academia.

インストールには管理者権限で yum install パッケージ名 とします。
例::

	# yum install clips-emacs
	...
	================================================================================
	 Package         アーキテクチャ
				    バージョン                         リポジトリー
										   容量
	================================================================================
	インストール中:
	 clips-emacs     noarch     6.30.0-0.4.20090722svn.fc17        fedora      14 k

	トランザクションの要約
	================================================================================
	インストール  1 パッケージ

	総ダウンロード容量: 14 k
	インストール済み容量: 19 k
	これでいいですか? [y/N]y
        ...

あるパッケージを利用するのに必要な他のパッケージもあわせて自動的にインストールされ
ます。


.. _querypkg:

_`パッケージ情報の問合せ`
,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,

インストールされているパッケージについて、様々な問合せができます。

 インストールされているパッケージの一覧
	rpm -qa
 あるパッケージPに含まれるファイルの一覧
	rpm -ql P
 あるファイルFが所属するパッケージの名前
 	rpm -qf F
 あるパッケージPの説明
 	rpm -qi P

パッケージに対するソースコードを得る方法については
 :ref:`gettingsourcecode` にて説明します。

