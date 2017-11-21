.. _sourcetree:

ソースファイルの分類とuniversal-ctagsの例
=======================================================================

ソースツリーには多数の .c、.hファイルが含まれています。
役割に応じてそれらを分類しておくと、読解の助けとなります。

ここでは、典型的な分類方法を紹介します。
例として ~/upstream/ctags を参照します。

アプリケーションから独立した汎用のコード(ユーティリティ)
-----------------------------------------------------------------------

既に紹介した通り、ある程度の規模のアプリケーションを実現
する上で、C言語には様々なものが欠けています。それらを
埋めるためのコードが付属していることがあります。これらは
アプリケーションのドメインからある程度独立しています。
言いかえるとアプリケーションの知識無しで読解することができます。
逆に読解して意味を押えておくと、アプリケーション中枢を読む
ときの大きな助けとなります。

典型的なものに、追加のデータ構造を実現するコードがあります。
例えば長さが必要に応じて自動的に拡張される文字列型や配列型、
ハッシュテーブル、リンクトリスト、などです。

ctagsであれば次のファイルが該当します。

main/vstring.c, main/vstring.h

	自動的に拡張される文字列型(vString)

main/strlist.c,main/strlist.h

	自動的に拡張されるvStringの配列

args.c,args.h

	コマンドライン引数に含まれるオプションを
	処理するための関数とデータ構造

htable.c,htable.h

	ハッシュテーブル

アプリケーションの「仕掛け」部分と「バックエンド」部分
-----------------------------------------------------------------------

ある程度複雑なアプリケーションとなると、実装にあたりまず、まず問題領域
のモデルを設計するでしょう。ソースコード中にはこのモデルを反映したデー
タ構造とそれらを操作する関数があるはずです。これを仕掛けと呼ぶことにし
ます。

この仕掛けに従ってより具体的な処理を記述した部分をバックエンドと呼ぶこ
とにします。



ctagsであれば、仕掛けに関連するコードが main ディレクトリに、バックエ
ンドと呼ぶべきコードがparsrsディレクトリに、格納されています。

main/parse.cとmain/parse.hはctagsjno中枢となる仕掛けです。parse.h中に
parserDefinitionという型が定義されています。これは、入力言語の種類から
独立した(タグを採取するための)パーザの型です。

.. code-block:: c

    typedef struct {
	    /* defined by parser */
	    char* name;                    /* name of language */
	    ...
	    simpleParser parser;           /* simple parser (common case) */
	    rescanParser parser2;          /* rescanning parser (unusual case) */
	    ...
    };


この構造体型には関数ポインタを保持するフィールドがいくつかあります。

.. code-block:: c

    typedef enum {
	    RESCAN_NONE,   /* No rescan needed */
	    RESCAN_FAILED, /* Scan failed, clear out tags added, rescan */
	    RESCAN_APPEND  /* Scan succeeded, rescan */
    } rescanReason;

    ...
    typedef void (*simpleParser) (void);
    typedef rescanReason (*rescanParser) (const unsigned int passCount);

これらのフィールドは言語毎に用意されたバックエンドが提供する関数を指し示します。

parse.cのcreateTagsForFileにてこのoparserフィールドあるいはparser2フィールドの指し
示すコールバックを呼び出しています。

.. code-block:: c

    static rescanReason createTagsForFile (
		    const char *const fileName, const langType language,
		    const unsigned int passCount)
    {
	    rescanReason rescan = RESCAN_NONE;
	    Assert (0 <= language  &&  language < (int) LanguageCount);
	    if (fileOpen (fileName, language))
	    {
		    parserDefinition *const lang = LanguageTable [language];

		    Assert (lang->parser || lang->parser2);

		    if (LanguageTable [language]->useCork)
			    corkTagFile();

		    if (lang->parser != NULL)
			    lang->parser ();
		    else if (lang->parser2 != NULL)
			    rescan = lang->parser2 (passCount);

		    makeFileTag (fileName);

		    if (LanguageTable [language]->useCork)
			    uncorkTagFile();

		    fileClose ();
	    }

	    return rescan;
    }


languageパラメータはどの言語を対象とするか、言いかえるとどのバックエンドを使うかを
指定しています。LanguageTable配列にバックエンドがならんでいます。
languageパラメータは、配列のインデックスとしてバックエンドを特定します。
LanguageTable配列からparserDefinition型のデータへのポインタを取り出して、
langローカル変数に保持します。そして、次のコードでバックエンドを呼び出しています。

.. code-block:: c

		    if (lang->parser != NULL)
			    lang->parser ();
		    else if (lang->parser2 != NULL)
			    rescan = lang->parser2 (passCount);

次にバックエンドのコードを一つ紹介します。 Makefileからタグを取り出すための
コードがparsers/make.cに記載されています。

.. code-block:: c

    extern parserDefinition* MakefileParser (void)
    {
	    static const char *const patterns [] = { "[Mm]akefile", "GNUmakefile", NULL };
	    static const char *const extensions [] = { "mak", "mk", NULL };
	    parserDefinition* const def = parserNew ("Make");
	    def->kinds      = MakeKinds;
	    def->kindCount  = COUNT_ARRAY (MakeKinds);
	    def->patterns   = patterns;
	    def->extensions = extensions;
	    def->parser     = findMakeTags;
	    return def;
    }

このうち以下の箇所で、Makefileをパースするための関数(findMakeTags)への
ポインタを「仕掛け」が提供する parserフィールドに、指定しています。

.. code-block:: c

		def->parser     = findMakeTags;
