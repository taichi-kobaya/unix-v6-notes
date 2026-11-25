/*
*forkシステムコールのカーネル側エントリポイント
*K＆R Cスタイルのため、引数リスト・戻り値型（int）が省略されている*
*/
fork()
{
    /*registerは変数をCPUレジスタに割り当てるようコンパイラへ促す指定*/
    register struct proc *p1, *p2;

    /* u は現在実行中プロセスのuser構造体。u_procpは自身のproc構造体へのポインタ（親側）*/
    p1 = u.u_procp; // 実行プロセス（親プロセス）のproc構造体をp1に格納

    /*procテーブル（全NPROCエントリ）を走査し、未使用（p_stat == NULL)スロットを探す*/
    for(p2 = &proc[0]; p2 < &proc[NPROC]; p2++)
        if(p2->p_stat == NULL)
            goto found;

    /*空きプロセススロットがない場合はEAGAINを設定して終了処理へ*/
    u.u_error = EAGAIN;
    goto out;

found:
    /*
    *newproc()でプロセス複製を行う。
    *複製後は親・子の両方がこの分岐を通過し、戻り値によって処理を分ける。
    * */
    if(newproc()) { // 親プロセスにはnewproc()から0が返って来る。よって偽になる。
        /* --- 子プロセス側の処理 --- */
        /*u_ar0はシステムコール復帰時にユーザー側レジスタへ書き戻される保存領域*/
        u.u_ar0[R0] = p1->p_pid;

        /*親から引き継いだ子プロセス累積実行時間・自身の実行時間を０に初期化*/
        u.u_cstime[0] = 0;
        u.u_cstime[1] = 0;
        u.u_stime = 0;
        u.u_cutime[0] = 0;
        u.u_cutime[1] = 0;
        u.u_utime[1] = 0;
        return;
    }
    /* --- 親プロセス側の処理 --- */
    /*生成された子プロセスのPIDをシステムコールの戻り値（R0）に設定*/
    u.u_ar0[R0] = p2->p_pid;

out:
    /*
    * =+は現在の+=にあたる古い複合代入記法
    *R7（PC）を調整し、システムコール復帰パスへ抜ける
    * */
    u.u_ar0[R7] = =+ 2;
}
