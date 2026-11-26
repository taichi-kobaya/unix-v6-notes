newproc() 
{
    int a1, a2;
    struct proc *p, *up;
    register struct proc *rpp;
    register *rip, n;

    p = NULL;

/* まず、プロセス用のスロットを見つけ、
* このプロセスから必要な情報をコピーする。
* forkが既にスロットの存在を確認しているため、パニックは発生しない。
*/
retry:
    mpid++;
    if(mpid < 0) {
        mpid = 0;
        goto retry;
    }
    for(rpp =&proc[0]; rpp < &proc[NPROC]; rpp++) {
        if(rpp->p_stat == NULL && p==NULL)
            p = rpp;
        if (rpp->p_pid==mpid)
            goto retry;
    }
    if ((rpp = p)==NULL)
        panic("no procs");

    /*
    *新しいプロセスのエントリを作成する
    */
    rip = u.u_procp;
    up = rip;
    rpp->p_stat = SRUN;
    rpp->p_flag = SLOAD;
    rpp->p_uid = rip->p_uid;    
    rpp->p_ttyp = rip->p_ttyp;
    rpp->p_nice = rip->p_nice;
    rpp->p_textp = rip->p_testp;
    rpp->p_pid = mpid;
    rpp->p_ppid = rip->p_pid;
    rpp->p_time = 0;

    /*
    *必要に応じて重複エントリを作成する
    */
    for(rip = &u.u_ofile[0]; rip < &u.u_ofile[NOFILE];)
        if(rpp = *rip++) != NULL)
            rpp->f_count++;
    if((rpp=up->textp) != NULL) {
        rpp->xcount++;
        rpp->xccount++;
    }
    u.u_cdir->i_count++;

    /*
    *新しいプロセスの環境を部分的にシミュレートする
    *それによって実際に作成された時（コピーによって作成された時）に、
    *正しく表示されるようにする。
    */
    savu(u.u_rsav);
    rpp = p;
    u.u_procp = rpp;
    rip = up;
    n = rip->p_size;
    a1 = rip->p_addr;
    rpp->p_size = n;
    a = malloc(coremap, n);

    /*
    *新しいプロセスに必要なコアが不足している場合、
    *現在のプロセスをスワップアウトしてコピーを作成する
    */
   if(a2 == NULL) {
    rip->p_stat =SIDL;
    rpp->p_addr = a1;
    savu(u.u_ssav);
    xswap(rpp, 0, 0);
    rpp->p_flag =| SSWAP;   // 既に立っているフラグ（ビット）は残したまま指定したフラグだけを新しくONにする
    rip->p_stat = SRUN;
   } else {
    /*
    *コアが十分にあるので、コピーのみを実行する
    */
        rpp->p_addr = a2;
        while(n--)
            copyseg(a1++, a2++);
   }
   u.u_procp = rip;
   return(0);
}