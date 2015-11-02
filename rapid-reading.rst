.. _rapid-reading:

.. 読む順番の話

効率の良いコードリーディング
========================================================================

「効率の良いコードリーディング」というものはありません。コードリーディ
ングは地道で単調で孤独な作業です。効率の良いことをやろうとすると思わぬ
見落しをして誤った結論を引き出してしまうことがあります。リスクと効率は
トレードオフの関係にあると考えて下さい。受講生のみなさんにはもう少し苦
労を味わって頂いてから、効率についてお話したかったのですが、演習の日程
には限りがありますのでここで、「効率の良いコードリーディング」について
説明しようと思います。

筆者は「効率の良いコードリーディング」とは追跡すべきフローの数を
経験に基いて積極的に限定しつつ読み進める作業であると考えます。繰り返し
になりますが、積極的に限定するので、限定しすぎて追跡すべきフローを見落
すリスクがあります。

結局、繰り返し読んで対象ソースコードに慣れ親しみ(リーディング資産を
増やし)、バランス感覚を掴むしかないように思います。

とは言え、限定せずに読むわけにもいかないので、フローを限定する方法を紹
介します。

.. TODO 重み付け, 関数の形状

* 枝刈り(cut)
* 間引き(filter)
* 縮退(reduce)
* 薄切り(slice)
* 関数の形状に基づく重み付け?(bet?)
* コードパターンに基づくジャンプ(jump)
  
それぞれは完全に独立しているわけではなく、これらを組合せて
追跡すべきフローの数を限定することになります。

枝刈り(cut)
------------------------------------------------------------------------
条件文の条件部分を見て興味のない分岐を読み飛ばします。

.. code-block:: c
	
	if (関心の無いように見える条件) 
	{
		A();
        } 
	else 
	{
		B();
	}

このようなコードがあった場合、まずifの条件を見て、
今解決しようとしている疑問との関係を考えます。
関係が無いと確信できれば、A()の部分を読み飛ばして、
あたかも以下のようなコードであると想定して読みます。

.. code-block:: c

	B();	

逆に条件部分を見て興味のある分岐以外を読み飛ばす、という表現が
適当な場合もあります。

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

このようなコードがあった場合、A()の部分を読み飛ばして、
あたかも以下のようなコードであると想定して読みます。

.. code-block:: c

	E();	
	D();

ここまではあたり前のことかもしれません。読み飛ばすこと自体にはリスクは
ありません。

問題となるのは「関心の無いように見える条件」
や「よくわからない定数1」、「よくわからない定数2」をどの程度まじめに調
査するか、ということです。きっちりとやるのであれば、「関心の無い条件」
についてフローを追跡する必要があります。「よくわからない定数1」、「よく
わからない定数2」についてプログラム全体での役割を調べる必要があります。

この部分の手を抜いて読み飛ばすと速くプルーフポイントに到達できるかもしれ
ません。かわりにプルーフポイントに到達できない、あるいは部分的な結論し
か得られないリスクがあります。

例
,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,
次に示すのはlinuxカーネルのI/O処理の一部です。I/Oリクエストの結果を
評価する部分です。読解者は、I/Oのおおまかな仕掛けを把握したいと考えて
いたとします。ローカル変数rに結果が保持されていることは想像がつきます。

.. code-block:: c

    static void dm_done(struct request *clone, int error, bool mapped)
    {
	    int r = error;
	    struct dm_rq_target_io *tio = clone->end_io_data;
	    dm_request_endio_fn rq_end_io = tio->ti->type->rq_end_io;

	    if (mapped && rq_end_io)
		    r = rq_end_io(tio->ti, clone, error, &tio->info);

	    if (r <= 0)
		    /* The target wants to complete the I/O */
		    dm_end_request(clone, r);
	    else if (r == DM_ENDIO_INCOMPLETE)
		    /* The target will handle the I/O */
		    return;
	    else if (r == DM_ENDIO_REQUEUE)
		    /* The target wants to requeue the I/O */
		    dm_requeue_unmapped_request(clone);
	    else {
		    DMWARN("unimplemented target endio return value: %d", r);
		    BUG();
	    }
    }
    /* 出典: linux/drivers/md/dm.c */

おおまかな仕掛けがわかれば良いと考えていたので、特殊な異常ケースを扱っているように見えた
r == DM_ENDIO_INCOMPLETEの分岐とr == DM_ENDIO_REQUEUE分岐を読まないことにしました。
elseの部分についてはその内容から、これも異常ケースを扱っていると考えdm_end_request
だけを読みました。

後から、rを返すrq_end_io(が指す)関数の定義を読んだりや
2つの定数(DM_ENDIO_INCOMPLETEとDM_ENDIO_REQUEUE)の役割りを知ったところで、
この読み飛ばした2つの分岐が「おおまかな仕掛け」として重要であることがわかりました。
枝刈りをしすぎました。

異常ケースを追っている場合は正常ケースを、正常ケースを追っている場合は異常ケースを
読み飛ばすのは良くやることです。しかしあるコードが異常ケースなのか正常ケースなのか、
というのは、読む範囲によって変ってきます。

TIPS
,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,
Z()部分を読むのに、ものすごく長いコードの末尾に移動したくなります。

.. code-block:: c
	
	if (関心の無いように見える条件) 
	{
		ものすごく長いコード
		....
		else
		...
		ものすごく長いコード
        } 
	else 
	{
		Z();
	}

エディタによっては、対応する開き括弧と閉じ括弧を移動するコマンドを持っているも
のがあるので、それを活用すると良いでしょう。(emacsの場合 C-M-f, C-M-b)


間引き(filter)
------------------------------------------------------------------------
理解しようとしていることに対して影響の無い文を読み飛ばします。
次の2つのケースが思いあたりました。

単一のスレッドによって実行される処理にだけ関心がある場合
	クリティカルセクションを保護するロック取得、開放関数の呼び出しを
	読み飛ばす。

痕跡文字列としてログやエラー出力を追っていない場合
	開発者向けデバッグトレース関数の呼び出しを読み飛ばす。

例: ロック処理の無視
,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,
.. code-block:: c

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

ここでクリティカルセクションを保護する目的で配置された
spin_lock_irqsaveとspin_unlock_irqrestoreを無視して、あたかも次のよう
なコードであると想定できます。

.. code-block:: c

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

間引くには具体的な関数名を知っている必要があります。
ここではspin_lock_irqsaveとspin_unlock_irqrestoreが(ある程度名前から自明ですが)
がロック処理を担当していることを知っていなければ間引けません。

逆が逆の場合、すなわちマルチスレッド処理に特に関心がある場合、クリティカルセクション
であることを示唆するロック、アンロックに囲まれた部分に注目します。

.. code-block:: c

	    spin_lock_irqsave(q->queue_lock, flags);
	    if (elv_queue_empty(q))
		    blk_plug_device(q);
	    blk_requeue_request(q, rq);
	    spin_unlock_irqrestore(q->queue_lock, flags);

qが共通にアクセスされる単位であることが読み取れます。

例: デバッグトレースの無視
,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,
デバッグ/トレース出力自体の出所を追っているのでなければ、その出力処理は無視
できるはずです。

.. code-block:: c

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

pr_debugの呼び出しを無視すれば、あたかも次のようなコードであると想定できます。

.. code-block:: c

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

ロック処理を無視した時と同様に、デバッグ出力用の関数の名前がpr_debugで
あることを知っている必要があります。

逆にデバッグ/トレース出力自体の出所を追っているのであれば、pr_debugの呼び出し
こそ注目すべき箇所です。

このように何に関心があるかによって間引きの対象が変わります。

縮退(reduction)
------------------------------------------------------------------------
モデルの説明では、関数がそれほど長くない(300行ぐらいまで)と暗に想定していました。
出現する関数が短ければ制御に関しては関数の呼び出し関係を追うことで理解を進めて
行くことができます。

想定しているよりも関数が長い場合があります。どうにもならない関数はあり
ます。ただ読むことしかありません。しかし読む見てみると関数の中が意味的
に分割されている場合があります。開発の都合で大きく変更したくなかっため
か、別の関数に切り出すことをしなかっただけで、ある程度処理が独立した行
(文、式)のか片間を見出せることがあります。こういった塊は頭の中で一つの
関数に置き換えて読み進めると良いでしょう。

.. ghostscript-8.70/psi/interp.c中のinterp関数
.. 	1000行程度(NAIST在学中最長)

sendmail-8.14.4/sendmail/deliver.cのdeliver関数
	2400行程度

この中であれば、例えば

.. code-block:: c

	if (bitset(EF_RESPONSE, e->e_flags))
	{
		macdefine(&e->e_macro, A_PERM, macid("{client_name}"), "");
		macdefine(&e->e_macro, A_PERM, macid("{client_ptr}"), "");
		macdefine(&e->e_macro, A_PERM, macid("{client_addr}"), "");
		macdefine(&e->e_macro, A_PERM, macid("{client_port}"), "");
		macdefine(&e->e_macro, A_PERM, macid("{client_resolve}"), "");
	}

という箇所があります。これは5つの設定変数に空文字を設定しているように読めます。
今のところで設定変数がどのように使われるのか関心が無ければ、頭の中で記述を以下の
ように1つの関数に閉じ込めてしまいます。

.. code-block:: c

	if (bitset(EF_RESPONSE, e->e_flags))
		macdefine5();

別の箇所に

.. code-block:: c

		else if (pid == 0)
		{
			int save_errno;
			int sff;
			int new_euid = NO_UID;
			int new_ruid = NO_UID;
			int new_gid = NO_GID;
			char *user = NULL;
			struct stat stb;
			extern int DtableSize;

			CurrentPid = getpid();

			/* clear the events to turn off SIGALRMs */
			sm_clear_events();

			/* Reset global flags */
			RestartRequest = NULL;
			RestartWorkGroup = false;
			ShutdownRequest = NULL;
			PendingSignal = 0;

			if (e->e_lockfp != NULL)
				(void) close(sm_io_getinfo(e->e_lockfp,
							   SM_IO_WHAT_FD,
							   NULL));

			/* child -- set up input & exec mailer */
			(void) sm_signal(SIGALRM, sm_signal_noop);
			(void) sm_signal(SIGCHLD, SIG_DFL);
			(void) sm_signal(SIGHUP, SIG_IGN);
			(void) sm_signal(SIGINT, SIG_IGN);
			(void) sm_signal(SIGTERM, SIG_DFL);
		...

という記述があります。これは新しいプロセスを起動した直後の処理です。
様々な変数の初期化やリソースの取り扱いを変更するためのシステムコール
呼び出しがなされています。とりあえずこういったものの頭の中で一つの
関数に閉じ込めてしまえます。


.. code-block:: c

		else if (pid == 0)
		{
			init_child_process();

.. 経験した範囲では関数が長くなる理由には、処理の内容が本質的に短くする
.. のが難しいものと、拡張の繰り返しによるものの2種類あるようです。

.. インタプリタのメインループは効率を追う都合もあり、一つの関数内に
.. goto文を多数配置するような処理になっています。
	

薄切り(slice)
------------------------------------------------------------------------
データフローを追跡していて特に着目している変数があれば、その変数の値を消費、供給
している箇所以外を無視します。無視することで選出されたコードをその変数のスライス
と呼ぶことにします [#slice]_  。

.. [#slice] 
   ここでスライスと言っているのは筆者が勝手につけた名前です。プログラム
   スライスからアイデアを得て名前をつけましたが、以降の説明がプログラム
   スライスの定義と一致していると期待しないで下さい。
   
この方法は、主に引数の消費箇所と返り値の供給箇所を読むときに使います。

引数の消費箇所
,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,

次に示すのはブロックデバイスに関連したデータ構造の開放処理です。

.. code-block:: c

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

wait引数がどのような意味を持つのかにだけ興味があれば、

.. code-block:: c

	    if (wait)
		    while (atomic_read(&md->holders))
			    msleep(1);

の箇所だけを読めば良いでしょう。

引数経由での値の供給
,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,

次のコードで引数resultに興味があるとします。

.. code-block:: c

    int
    gs_pop_string(gs_main_instance * minst, gs_string * result)
    {
	i_ctx_t *i_ctx_p = minst->i_ctx_p;
	ref vref;
	int code = pop_value(i_ctx_p, &vref);

	if (code < 0)
	    return code;
	switch (r_type(&vref)) {
	    case t_name:
		name_string_ref(minst->heap, &vref, &vref);
		code = 1;
		goto rstr;
	    case t_string:
		code = (r_has_attr(&vref, a_write) ? 0 : 1);
	      rstr:result->data = vref.value.bytes;
		result->size = r_size(&vref);
		break;
	    default:
		return_error(e_typecheck);
	}
	ref_stack_pop(&o_stack, 1);
	return code;
    }
    /* 出典: ghostscript-8.70/psi/imain.c */

このケースではresultの値は消費されるのではなく、更新されています。

resultに関心がある場合着目するのは以下の箇所です。

.. code-block:: c

	      rstr:result->data = vref.value.bytes;
		result->size = r_size(&vref);

の箇所を見ればdataフィールドとsizeフィールドの値が更新されてる
ことがわかります。

呼び出し元は、次のように引数を与えて呼び出していると想像できます。

.. code-block:: c

        gs_string s;
	int r;
	...	
	r = gs_pop_string(i, &s);
	...

この例では追跡するべきフローを大幅に減らすことはできません。

.. code-block:: c

	      rstr:result->data = vref.value.bytes;
		result->size = r_size(&vref);

結局vrefについて調べなければ、resultの値の出自を説明できない
からです。

返り値経由での値の供給
,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,

関数の返り値に関心があるのであれば、まずreturn文を探します。

.. code-block:: c

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

return文に指定された値が即値や定数であれば、それで目的を達成したこと
になります。変数が指定されている場合、その変数を変更している箇所を
探すことになります。

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


C言語の文法を知っていれば自明なことですが、変更しているコードは
次のような形をしています。


* 代入系演算子

.. code-block:: c
	
	r = something;
	r += something;
	r -= something;
	r |= something;
	r &= something;
	...

* インクリメント演算子

.. code-block:: c
	
	r++;
	r--;

* 関数への参照渡し

.. code-block:: c
	
	func(&r);

* 別の変数を経由した間接的な変更

.. code-block:: c
	
	x = &r;
	...
	x->field = xxx;

.. slice+reduce

関数の形状に基づく重み付け?(bet?)
------------------------------------------------------------------------

(この方法に良い名前が思いあたりませんでした。)

関数の中の論理構造は様々ですが、極端に「均一」なものや、「偏った」もの
があります。偏ったものについては当りをつけて後ろから読んだ方が意図が掴
みやすいことがあります。

均一な関数の例:

.. code-block:: c

    static void __init do_basic_setup(void)
    {
	    cpuset_init_smp();
	    usermodehelper_init();
	    shmem_init();
	    driver_init();
	    init_irq_proc();
	    do_ctors();
	    usermodehelper_enable();
	    do_initcalls();
	    random_int_secret_init();
    }
		
     /* 出典: linux/main/init.c */

均一な関数は、読む箇所を限定できません。先頭から見て行く必要
があります。

偏っている関数の例

.. code-block:: c
		
    static long __init do_utime(char *filename, time_t mtime)
    {
	    struct timespec t[2];

	    t[0].tv_sec = mtime;
	    t[0].tv_nsec = 0;
	    t[1].tv_sec = mtime;
	    t[1].tv_nsec = 0;

	    return do_utimes(AT_FDCWD, filename, t, AT_SYMLINK_NOFOLLOW);
    }

これは入力パラメータを本当に呼び出したい後続の関数に引き渡すことができ
るよう変換している前半と、変換結果を引数に別の関数(本命関数)を呼び出し
ている後半に分れています。順方向に制御フローを追跡しているのであれば、
前半を無視してまず本命関数を呼び出す箇所を見ただけで、対象の関数が何を
しようとしているおおまかにわかります。本命関数に渡されている変数
の名前に着目して、スライスする変数を選ぶのも良いでしょう。

パラメータを変数する以外に、適切な値を持つパラメータが渡された場合に
限って後続の関数を呼び出す、という役割りの関数にも偏りが見られます。

.. code-block:: c

    /* Opens existing queue */
    static struct file *do_open(struct path *path, int oflag)
    {
	    static const int oflag2acc[O_ACCMODE] = { MAY_READ, MAY_WRITE,
						      MAY_READ | MAY_WRITE };
	    int acc;
	    if ((oflag & O_ACCMODE) == (O_RDWR | O_WRONLY))
		    return ERR_PTR(-EINVAL);
	    acc = oflag2acc[oflag & O_ACCMODE];
	    if (inode_permission(d_inode(path->dentry), acc))
		    return ERR_PTR(-EACCES);
	    return dentry_open(path, oflag, current_cred());
    }

do_openの役割りはdentry_openを呼び出すことですが、その前に
自身に渡されたパラメータの値を調べて、適当でなければエラーとともに
制御を呼び出し元に返却しています。

.. 前倒し、複数->単数、中出っ張り

コードパターンに基づくジャンプ(jump)
------------------------------------------------------------------------

異なるソフトウェアであっても良く似たコード群(あるいは対、組)
(コードパターン)が出てくることがあります。

読解中にあるコードパターンの断片が使われていることに気付けば、
パターンの他の部分に対応するコードがどこかにあると期待できます。
他のコードを一切読み飛ばして「他の部分」を読むことで目的の
情報を得ることができるかもしれません。少なくとも、今後どう
いったコードが出てきそうかの予測が立つだけでも、読解の助け
となります。



以降では具体的なコードパターンを紹介していきます。

.. ・Technical knowledge transfer for OSS Source Code Reading Practice with developing a document described below.
.. - Objectives of Code Reading
.. - Guidelines of Code Reading
.. - How to use search tools (including exercise)
.. - C Language overview (including exercise)
.. - Program execution environment
.. - Code Reading #1 : Resource release and allocation (including exercise)
.. - Code Reading #2 : Function Pointer and Call Back Function (including exercise)
.. - Code Reading #3 : Implementation of Object System by C Language (including exercise)

.. ・以下内容のOSSのコードリーディング演習を行う。
.. - コードリーディングの意義・目的
.. - コードリーディングの指針
.. - 検索ツールの使い方(演習課題説明を含む)
.. - C言語について(演習課題説明を含む)
.. - プログラムの実行環境について
.. - 読解のパターン1: リソースの開放と確保(演習課題説明を含む)
.. - 読解のパターン2: 関数ポインタとコールバック関数(演習課題説明を含む)
.. - 読解のパターン3: C言語によるオブジェクトシステムの実装(演習課題説明を含む)

