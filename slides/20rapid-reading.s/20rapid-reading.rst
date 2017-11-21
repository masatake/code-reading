-------------------------------------------------------------------------
効率の良いコードリーディング
-------------------------------------------------------------------------

.. include:: ../header.rst

前提
-------------------------------------------------------------------------
* 「効率の良いコードリーディング」というものはない。
* 効率と精度はトレードオフの関係になる。
* 読む箇所を絞り込むと見落しが発生する。

方法
-------------------------------------------------------------------------
経験に基いて追跡すべきフローの数を限定する。

* コードパターンに基づくジャンプ(jump)
* 枝刈り(cut)
* 間引き(filter)
* 縮退(reduce)
* 薄切り(slice)


リーディング資産が豊富であれば適切にフローを限定できる。

コードパターンに基づくジャンプ(jump)
------------------------------------------------------------------------
異なるソフトウェアであっても良く似たコード群(あるいは対、組)
(コードパターン)が出てくる。

.. figure:: ../../pattern.svg

読解中に既知のコードパターンの出現に気付けば、コード群の他の部分を意識
的に探し、みつかればそこをベースポイントとすることができる。

枝刈り (1/2)
------------------------------------------------------------------------
条件文の条件部分を見て、興味のない分岐を読み飛す。

.. code-block:: c

	if (関心の無いように見える条件)
	{
		A();
	}
	else
	{
		B();
	}

.. raw:: pdf

	PageBreak  oneColumn

これを次のコードと等価であるとして読む。

.. code-block:: c

	B();

----

デモ
  ブロックが長大な場合エディタの対応括弧間移動コマンドを使うと良い。

枝刈り (2/2)
------------------------------------------------------------------------
条件部分を見て興味のある分岐以外を読み飛す。

.. code-block:: c

	swtich (var) {
	case よくわからない定数1:
		C();
		break;
	case 関心のある定数:
		E();
	case よくわからない定数2:
		D();
		break;
	default:
		F();
		break;
	}

.. raw:: pdf

	PageBreak  oneColumn


これを次のコードと等価であるとして読む。

.. code-block:: c

	E();
	D();


間引き(filter)
------------------------------------------------------------------------
理解しようとしていることに対して影響の無い文を読み飛ばす。

間引き(filter)
------------------------------------------------------------------------
例(1)
........................................................................
単一のスレッドによって実行される処理にだけ関心がある場合、クリティカルセクションを
保護するロック取得、開放関数の呼び出しを読み飛ばす。

.. code-block:: c
    :linenos:

    void dm_requeue_unmapped_request(struct request *clone)
    {
	    int rw = rq_data_dir(clone);
	    struct dm_rq_target_io *tio = clone->end_io_data;
	    struct mapped_device *md = tio->md;
	    struct request *rq = tio->orig;
	    struct request_queue *q = rq->q;
	    unsigned long flags;

	    dm_unprep_request(rq);

	    spin_lock_irqsave(q->queue_lock, flags);
	    if (elv_queue_empty(q))
		    blk_plug_device(q);
	    blk_requeue_request(q, rq);
	    spin_unlock_irqrestore(q->queue_lock, flags);

	    rq_completed(md, rw, 0);
    }
    /* 出典: linux/drivers/md/dm.c */

.. raw:: pdf

	PageBreak  oneColumn


これを次のコードと等価であるとして読む。

.. code-block:: c
    :linenos:

    void dm_requeue_unmapped_request(struct request *clone)
    {
	    int rw = rq_data_dir(clone);
	    struct dm_rq_target_io *tio = clone->end_io_data;
	    struct mapped_device *md = tio->md;
	    struct request *rq = tio->orig;
	    struct request_queue *q = rq->q;

	    dm_unprep_request(rq);

	    if (elv_queue_empty(q))
		    blk_plug_device(q);
	    blk_requeue_request(q, rq);

	    rq_completed(md, rw, 0);
    }


間引き(filter)
------------------------------------------------------------------------
例(2)
........................................................................
痕跡文字列としてログやエラー出力を追っていない場合、開発者向けデバッグトレース
関数の呼び出しを読み飛ばす。

.. code-block:: c
    :linenos:

    static int bond_netdev_event(struct notifier_block *this,
				 unsigned long event, void *ptr)
    {
	    struct net_device *event_dev = (struct net_device *)ptr;

	    if (dev_net(event_dev) != &init_net)
		    return NOTIFY_DONE;

	    pr_debug("event_dev: %s, event: %lx\n",
		    (event_dev ? event_dev->name : "None"),
		    event);

	    if (!(event_dev->priv_flags & IFF_BONDING))
		    return NOTIFY_DONE;

	    if (event_dev->flags & IFF_MASTER) {
		    pr_debug("IFF_MASTER\n");
		    return bond_master_netdev_event(event, event_dev);
	    }

	    if (event_dev->flags & IFF_SLAVE) {
		    pr_debug("IFF_SLAVE\n");
		    return bond_slave_netdev_event(event, event_dev);
	    }
	    return NOTIFY_DONE;
    }
    /* 出典: linux/drivers/net/bonding/bond_main.c */

.. raw:: pdf

	PageBreak  oneColumn

これを次のコードと等価であるとして読む。

.. code-block:: c
    :linenos:

    static int bond_netdev_event(struct notifier_block *this,
				 unsigned long event, void *ptr)
    {
	    struct net_device *event_dev = (struct net_device *)ptr;

	    if (dev_net(event_dev) != &init_net)
		    return NOTIFY_DONE;

	    if (!(event_dev->priv_flags & IFF_BONDING))
		    return NOTIFY_DONE;

	    if (event_dev->flags & IFF_MASTER) {
		    return bond_master_netdev_event(event, event_dev);
	    }

	    if (event_dev->flags & IFF_SLAVE) {
		    return bond_slave_netdev_event(event, event_dev);
	    }
	    return NOTIFY_DONE;
    }


縮退(reduction)
------------------------------------------------------------------------
関数中の文を意味単位でグループ化して、グループを一つの関数呼び出しとして
詳細を隠蔽して読む。

.. code-block:: c
	:linenos:

	...
	if (bitset(EF_RESPONSE, e->e_flags))
	{
		macdefine(&e->e_macro, A_PERM, macid("{client_name}"), "");
		macdefine(&e->e_macro, A_PERM, macid("{client_ptr}"), "");
		macdefine(&e->e_macro, A_PERM, macid("{client_addr}"), "");
		macdefine(&e->e_macro, A_PERM, macid("{client_port}"), "");
		macdefine(&e->e_macro, A_PERM, macid("{client_resolve}"), "");
	}
	...

	/* 出典: sendmail-8.14.4/sendmail/deliver.cのdeliver関数
	   (2400行の関数!) */

.. raw:: pdf

	PageBreak  oneColumn

これを次のコードと等価であるとして読む。

.. code-block:: c
	:linenos:

	if (bitset(EF_RESPONSE, e->e_flags))
		macdefine5();

ここでmacdefine5とはソースコードの読解者が勝手に頭の中で定義した
関数の名前である。

薄切り(slice)
------------------------------------------------------------------------
* データフローを追跡していて特に着目している変数があれば、その変数の値を消費、供給
  している箇所以外を無視する。
* 無視することで選出されたコードをその変数のスライスと呼ぶことにする。

薄切り(slice)
------------------------------------------------------------------------
例(1): 引数の消費箇所
........................................................................

wait引数がどのような意味を持つのかにだけ興味があるとする。

.. code-block:: c
    :linenos:

    static void __dm_destroy(struct mapped_device *md, bool wait)
    {
	    struct dm_table *map;

	    might_sleep();

	    spin_lock(&_minor_lock);
	    map = dm_get_live_table(md);
	    idr_replace(&_minor_idr, MINOR_ALLOCED, MINOR(disk_devt(dm_disk(md))));
	    set_bit(DMF_FREEING, &md->flags);
	    spin_unlock(&_minor_lock);

	    if (!dm_suspended_md(md)) {
		    dm_table_presuspend_targets(map);
		    dm_table_postsuspend_targets(map);
	    }

	    /*
	     * Rare, but there may be I/O requests still going to complete,
	     * for example.  Wait for all references to disappear.
	     * No one should increment the reference count of the mapped_device,
	     * after the mapped_device state becomes DMF_FREEING.
	     */
	    if (wait)
		    while (atomic_read(&md->holders))
			    msleep(1);
	    else if (atomic_read(&md->holders))
		    DMWARN("%s: Forcibly removing mapped_device still in use! (%d users)",
			   dm_device_name(md), atomic_read(&md->holders));

	    dm_sysfs_exit(md);
	    dm_table_put(map);
	    dm_table_destroy(__unbind(md));
	    free_dev(md);
    }

    /* 出典: linux/drivers/md/dm.c */

次のコードと等価であるとして読む。

.. code-block:: c
    :linenos:

    static void __dm_destroy(struct mapped_device *md, bool wait)
    {
	    if (wait)
		    while (atomic_read(&md->holders))
			    msleep(1);
    }

薄切り(slice)
------------------------------------------------------------------------
例(2): 返り値経由での値の供給
........................................................................

関数の返り値にだけ関心があるとする。

.. code-block:: c
    :linenos:

    static int dm_wait_for_completion(struct mapped_device *md, int interruptible)
    {
	    int r = 0;
	    DECLARE_WAITQUEUE(wait, current);

	    dm_unplug_all(md->queue);

	    add_wait_queue(&md->wait, &wait);

	    while (1) {
		    set_current_state(interruptible);

		    smp_mb();
		    if (!md_in_flight(md))
			    break;

		    if (interruptible == TASK_INTERRUPTIBLE &&
			signal_pending(current)) {
			    r = -EINTR;
			    break;
		    }

		    io_schedule();
	    }
	    set_current_state(TASK_RUNNING);

	    remove_wait_queue(&md->wait, &wait);

	    return r;
    }


次のコードと等価であるとして読む。

.. code-block:: c
    :linenos:

    static int dm_wait_for_completion(struct mapped_device *md, int interruptible)
    {
	    int r = 0;
	    while (1) {
		    if (interruptible == TASK_INTERRUPTIBLE &&
			signal_pending(current)) {
			    r = -EINTR;
			    break;
		    }
	    }
	    return r;
    }

コードパターンに基づくジャンプ(jump)
------------------------------------------------------------------------
異なるソフトウェアであっても良く似たコード群(あるいは対、組)
(コードパターン)が出てくる。

読解中に既知のコードパターンの出現に気付けば、コード群の他の部分を意識
的に探し、みつかればそこをベースポイントとすることができる。
