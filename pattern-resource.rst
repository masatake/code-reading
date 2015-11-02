.. _pattern_resource:

コードパターン: データ型にひも付く関数群
=======================================================================

「ある型Tのデータに対して、それを活用するための関数群が定義されている」
というのはコードパターンとして頻出します。この関数群は引数にT型のデータ
を取って、「活用」します。ただし以降に述べる構築関数だけは例外で、引数
に取るか代りにT型のデータを返します。

関数群の間にはなんらかの関係がある場合があります。例えば順序関係です。
ある関数Bを呼び出す前に関数Aを呼び出す必要がある、あるいは関数Cを呼び出
したら関数Dを呼び出すはずである、といった具合です。

このような知見は読解のヒントとなります。

データのライフサイクルに関連した関数の呼び出し順序
,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,
関数群はデータのライフサイクルの観点で次のように分類できることが多いです。

* 構築関数(初期化関数)
* 破壊関数(終末処理関数)
* その他(操作関数)

あるデータはまず構築関数を経て変数に結びつけられ、次に操作関数によって
活用されて、最後に破壊関数を経て変数から切り離されます。

.. figure:: resource-lifecycle.svg

この知見を読み飛ばしに使うことができます。

* 操作関数を呼び出している箇所に遭遇して、引数に与えらているそのデータ
  について出自を知りたければ(逆方向のデータフロー追跡)、構築関数を呼び出
  している箇所を探せば良いでしょう。

* 操作関数を呼び出している箇所に遭遇して、引数に与えらているそのデータ
  がいつまで使われるのかを知りたければ(順方向のデータフロー追跡)、
  破壊関数が呼び出されている箇所を探せば良いでしょう。

* 構築関数を呼び出している箇所に遭遇して、構築されたデータがどのように活用
  されているのか関心があれば、操作関数を呼び出している箇所を指せば良いでし
  ょう。
  
あるデータに対する関数群の存在は以下の理由で特定しやすいです。

* 同じヘッダファイル中で宣言されていることがある。
* 同じ.Cファイルの中で定義されていることがる。
* 関数の名前に共通のプレフィックスを持つことがある。

ただしデータのライフサイクル管理に参照計数を使っている
場合、あるいはプログラム終了時にOSによるデータ(リソース)の
開放を想定している場合には、破壊関数の呼び出しがみつから
ないかもしれません。

データのライフサイクルに基づいた読み飛ばしの例
.......................................................................

.. code-block:: c

        struct t {
                ...
		FILE* fp;
		...
	} T;
	...
	T->fp = fopen("関心のあるファイル", "r");
	...
	fgets(buffer, size, T->fp);
	do_something(buffer);
	...
	fclose(T->fp);
	...

.. figure:: resource-lifecycle-FILE.svg

* fopenが呼び出されていたら、どこかでFILE*の操作関数、例えば
  fprintfとかfgetsが呼び出されているはずです。

* fgetsの引数に渡しているFILE*の変数について出自を知りたければ、
  FILE*のデータの構築処理を担当するfopenを呼び出している箇所を探
  してみると良いでしょう。

* fgetsの引数に渡しているFILE*の変数について寿命を知りたければ、
  FILE*のデータの破壊処理を担当するfcloseを呼び出している箇所を探
  してみると良いでしょう。

FILE*を操作する多くの関数は、名前に共通のプレフィックス f を使います。

データのライフサイクルにより呼び出し順序のある関数群の例
.......................................................................

FILE*
	構築関数

		* fopen
		* ...

	操作関数

                * fgets
                * read
                * fwrite
		* fprintf
                * fscanf
		* ...

	破壊関数

		* fclose

DIR
	構築関数

		* opendir
		* ...

	操作関数

		* readdir
		* ...
		
	破壊関数

		* closedir

ソケット記述子(int)

        構築関数

		* socket
		* accept

	操作関数(サーバ側)

		* bind
		* listen
		* accept

	操作関数(クライアント側)

		* bind
		* connect

	操作関数

		* read, recv
		* write, send
		* send

	破壊処理

		* shutdown
		* close
	

その他の呼び出し順序のある関数群の例
,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,

動的メモリ

	1. malloc(や内部でmallocを呼び出す関数)で確保して
	2. freeで開放します。

	例 (xmallocとfree, そもそもfile_name_initとfile_name_freeが組になっている)

	.. code-block:: c

	    static void
	    file_name_free (struct file_name *p)
	    {
	      free (p->buf);
	      free (p);
	    }

	    static struct file_name *
	    file_name_init (void)
	    {
	      struct file_name *p = xmalloc (sizeof *p);

	      /* Start with a buffer larger than PATH_MAX, but beware of systems
		 on which PATH_MAX is very large -- e.g., INT_MAX.  */
	      p->n_alloc = MIN (2 * PATH_MAX, 32 * 1024);

	      p->buf = xmalloc (p->n_alloc);
	      p->start = p->buf + (p->n_alloc - 1);
	      p->start[0] = '\0';
	      return p;
	    }

	    /* 出典: coreutils/src/pwd.c */


pthread_mutex_t(ロック)

	1. pthread_mutex_lockあるいはpthread_mutex_trylockでロックして、
	2. pthread_mutex_unlockでアンロックします。

	例
	
	.. code-block:: c

	    /* Priority queue of merge nodes. */
	    struct merge_node_queue
	    {
	      struct heap *priority_queue;  /* Priority queue of merge tree nodes. */
	      pthread_mutex_t mutex;        /* Lock for queue operations. */
	      pthread_cond_t cond;          /* Conditional wait for empty queue to populate
					       when popping. */
	    };

	    ...

	    static void
	    queue_insert (struct merge_node_queue *queue, struct merge_node *node)
	    {
	      pthread_mutex_lock (&queue->mutex);
	      heap_insert (queue->priority_queue, node);
	      node->queued = true;
	      pthread_mutex_unlock (&queue->mutex);
	      pthread_cond_signal (&queue->cond);
	    }
	    /* 出典: coreutils/src/sort.c */
		
参照計数付きのデータ構造

	1. `ref`, `acquire`, `retain`, `get` といった名前のついた関数で参照を開始して、
	2. `unref`, `release`, `put` といった名前のついた関数で参照を解除します。

	例(try_module_getとmodule_put)

	.. code-block:: c

	    static int batadv_socket_open(struct inode *inode, struct file *file)
	    {
		    unsigned int i;
		    struct batadv_socket_client *socket_client;

		    if (!try_module_get(THIS_MODULE))
			    return -EBUSY;

		    nonseekable_open(inode, file);

		    socket_client = kmalloc(sizeof(*socket_client), GFP_KERNEL);
		    if (!socket_client) {
			    module_put(THIS_MODULE);
			    return -ENOMEM;
		    }

	      /* 出典 linux/net/batman-adv/icmp_socket.c */


.. 呼び出し順序に決まりのない関数群の例
.. ,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,

.. データフィールドや属性へのアクセス

.. 	* `*get*` で読み出す。
.. 	* `*set*` あるいは `*put*` で読み出す。

.. pthread_cond_t(条件変数)
..
.. 	* pthread_cond_signal, pthread_cond_broadcastで通知する。
.. 	* pthread_cond_waitで受信する。
