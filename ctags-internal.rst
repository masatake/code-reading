.. _ctags-internal:

Universal ctagsの内部紹介
=============================================

主要なデータ型
--------------------------------------------

parserDefinition

	定義箇所

		main/parse.h

	役割

		入力言語別に存在するパーザを表現する
		データ構造です。

	フィールド

		`name`

			パーザの名前、 `--list-languages` の
			出力に使われます。

		`parser` あるいは `parser2`

			実際にパースする関数へのポインタを保持します。
			2つのうちNULLで無い方が使われます。

		`initialize`

			このパーザが始めつ使われるとき、このフィールドが
			NULLで無ければ、このフィールドが指す関数が呼び出されます。
			パーザに何らかの初期化が必要な場合に使います。

		`extensions`

			このパーザが担当できるファイルの拡張子が列挙された
			文字列の配列を指し示します。`--list-maps` の出力にも
			使われます。

		`kinds`

			kindOption型 はkindを表現します。
			このフィールドは、パーザが扱うkindに対するkindOptionが列挙された
			配列を指し示します。

		`kindCount`

			`kinds` 配列の要素数を示します。

tagEntryInfo

	定義箇所

		main/parse.h

	役割

		tagsファイル中の各要素(行)を表現します。
		パーザはこのデータ構造を作って、
		makeSimpleTag あるいは makeTagEntryを
		呼び出します。すると出力されます。


	フィールド

		`name`

			ソースコード上に定義された「何か」の名前です。

		`sourceFileName`

			`name` が定義されたソースコードファイルの名前です。

		`lineNumber`

			`name` が定義されたソースコードファイル中の行番号です。

		`kind`

			「何か」の種類を示すkindOption型のデータを指します。

		`extensionFields`

			各種 field に対するデータを保持する構造体が埋め込まれています。
			課題に関連する `signature` もここに記録されます。


主要な関数 と マクロ
--------------------------------------------

createTagsForFile

	定義箇所

		main/parse.c

	役割

		パーザが使います。
		仕掛けの側にあって、入力ファイル毎にバックエンド(の
		parserあるいはparser2フィールドが指し示す関数)を呼び出
		します。

fileGetc

	定義箇所

		main/read.c

	役割

		パーザが使います。
		パーザには行単位で入力を扱いパースするものと、
		文字単位で入力を扱いパースするものがあります。

		呼び出されるたびに現在の入力ファイルから1文字を返します。

fileReadLine

	定義箇所

		main/read.c

	役割

		パーザが使います。
		呼び出されるたびに現在の入力ファイルから1行を返します。

initTagEntry/initTagEntryFull

	定義箇所

		main/entry.c

	役割

		パーザが使います。
		引数として渡したtagEntryInfo構造体の変数を初期化します。

makeTagEntry

	定義箇所

		main/entry.c

	役割

		パーザが使います。
		引数として渡したtagEntryInfo構造体の変数を書き出すよう、
		「仕掛け」の側に依頼します。

makeSimpleTag

	定義箇所

		main/parse.c

	役割

		initTagEntryの処理とmakeTagEntryの処理を
		まとめて実行します。


主要な変数
--------------------------------------------

Option

	定義箇所

		main/options.c

	役割

		コマンドライン引数で与えられたオプションの内容を保持し
		ています。大域変数なのでctagsのどこからでも参照できま
		す。





main関数の場所
--------------------------------------------

ctags-github/main/main.cの末尾


main関数の内容
--------------------------------------------

.. code-block:: c

    extern int main (int __unused__ argc, char **argv)
    {
	    cookedArgs *args;

	    setCurrentDirectory ();
	    setExecutableName (*argv++);
	    sanitizeEnviron ();
	    checkRegex ();

	    args = cArgNewFromArgv (argv);
	    previewFirstOption (args);
	    testEtagsInvocation ();
	    initializeParsing ();
	    initOptions ();
	    readOptionConfiguration ();
	    verbose ("Reading initial options from command line\n");
	    parseCmdlineOptions (args);
	    checkOptions ();
	    unifyLanguageMaps ();
	    makeTags (args);

	    /*  Clean up.
	     */
	    cArgDelete (args);
	    freeKeywordTable ();
	    freeRoutineResources ();
	    freeSourceFileResources ();
	    freeTagFileResources ();
	    freeOptionResources ();
	    freeParserResources ();
	    freeRegexResources ();
	    freeXcmdResources ();
    #ifdef HAVE_ICONV
	    freeEncodingResources ();
    #endif

	    if (Option.printLanguage)
		    return (Option.printLanguage == TRUE)? 0: 1;

	    exit (0);
	    return 0;
    }

`args = cArgNewFromArgv (argv);`

    OSから渡されてきたコマンドライン引数 `char **argv` を、自前で持ってい
    るオプション引数解析ルーチンに渡して、自分に都合の良いcookedArgs型へ変
    換しています。

`previewFirstOption (args);`

    引数として渡されたオプションのうち優先して解釈する必要のある
    ものを解釈、処理しています。

`initializeParsing ()`

    入力言語別に存在するパーザを LanguageTable という
    配列に登録します。各パーザを識別する整数値
    パーザが自身を登録するための関数があり、それら
    は parsers.h にPARSER_LISTマクロとして列挙され
    ています。


`parseCmdlineOptions ()`

     引数として渡されたオプション を解釈、処理します。

`makeTags (args);`

     ctagsの中枢となる関数で。
     argsに残された入力ファイルの一覧を引数にとり、引数一つ一つに対し
     て、引数が指し示すファイルを処理するのに適当なパーザを選択し、パー
     ザを呼び出します。
