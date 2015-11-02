.. _callback:

コードパターン: 関数ポインタとコールバック
=======================================================================

.. 関数ポインタの初期化の方法を書く(構造体の.)

はじめに
,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,

他のプログラムモジュールの処理の一部に独自の処理を組込む場合に関数
ポインタが使われます。関数ポインタがどのように使われるかをコードパター
ンとして知っておくと、読み飛ばしに使えます。

具体的には「関数ポインタを渡す処理」、「関数ポインタで指定された関数を
呼び出す処理」の2つの処理からなり、ソースコードを読んでいて、その一方に
遭遇した時点で、他方の処理を探してベースポイントとできます。

関数ポインタで指定された関数を呼び出す処理
,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,
処理を組込まれる側は、それを受けつけるための公開関数を提供しているはずです。

例

.. code-block:: c

	   void qsort(void *base, size_t nmemb, size_t size,
		      int(*compar)(const void *, const void *));

qsortはbaseで指定された配列の要素を並びかえます。並び換えの処理に必要
となる要素どうしの比較は、qsortの呼び出し元が与える引数comparが担当します。

void*型の引数を2つ取り、int型の値を返す関数へのポインタ(関数
ポインタ)をcomparで受けとります。


qsortの内部ではcomparを呼び出している処理があるはずです。

.. code-block:: c

	 if (compar((const void*)a, (const void*)b) < 0)
	 	...
	

あるいは

.. code-block:: c

	if ((*compar)((const void*)a, (const void*)b) < 0)
		...

と記載されているかもしれません。
関数ポインタ経由で指し示される関数をコールバック関数、
コールバック関数を呼び出すことをコールバックと言います。

関数ポインタを渡す処理
,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,

「独自の処理」を記述した関数(== コールバック関数)の参照を引数として
公開関数に渡している箇所があるはずです。

コールバック関数のシグネチャーは、公開関数が期待するものと一致している
はずです。

.. code-block:: c

        int *array;
	
	int less_than(const void* a, const void* b)
	{
		int A = *(int *)a;
		int B = *(int *)b;

		if (A == B) return 0;
		elif (A < B) return 1;
		else return -1;
	}
	
このような関数がqsortに渡されます。

.. code-block:: c

	qsort(array, array_length, sizeof(int), lesst_than);

あるいは

.. code-block:: c

	qsort(array, array_length, sizeof(int), &lesst_than);

と記載されているかもしれません。

コールバック関数の型
,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,

処理を組込まれる側は、公開関数の宣言とともに、受けとるコールバック関数の型を
typedefしていることがあります。

.. code-block:: c

	typedef int(* CompareFunc)(const void *, const void *);

	void qsort(void *base, size_t nmemb, size_t size,
	           CompareFunc compar);

	
コードパターンの拡張: 追加の引数
,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,

コールバック関数が定義された側に由来するデータを、コールバック関数の処理で
必要とする場合があります。大域変数やファイルスコープ変数で実現できますが、
広いスコープの変数の導入を避けるため、あるいは公開関数の呼び出しのたびに異
なるデータを渡せるように、引数を通してデータをコールバック関数に渡すことが
できるよう公開関数のインターフェイスが拡張されている場合があります。

このように引数経由でコールバック関数に渡すデータをユーザデータ、あるいは
クライアントデータと言います。ユーザデータの型にはvoid \*型が使われます。

擬似的にユーザデータを受けとるようqsortを拡張したqsort_xを考えてみます。

.. code-block:: c

	   void qsort_x(void *base, size_t nmemb, size_t size,
		        int(*compar_x)(const void *, const void *, void *),
			void* user_data);
	    ....

qsort_xの内部ではcompar_xを呼び出す記述は次のようになります、

.. code-block:: c

	 if (compar_x((const void*)a, (const void*)b, user_data) < 0)
	 	...

呼び出し側は、コールバック関数が必要とするデータを詰め合せた構造体変数を
用意して、void*にキャストして公開関数に渡します。

.. code-block:: c

	struct DataForQsort {
		int x;
	};

	int less_than(const void* a, const void* b, void* data);

	...

	struct DataForQsort data;
	data.x = n;
	...

	qsort_x(array, array_length, sizeof(int), lesst_than_x, &data);
	
	...

	int less_than(const void* a, const void* b, void* data) 
	{
		struct DataForQsort * qdata = (struct DataForQsort *)data;
		...
	}
	
	
.. コードパターンの拡張: 遅延処理
.. ,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,

.. コードパターンの拡張: 返り値
.. ,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,
