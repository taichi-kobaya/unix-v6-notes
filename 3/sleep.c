sleep(chan, pri)
{
    register *rp, s;

    s = PS->integ;
    rp = u.u_procp;
    if(pri >= 0) {
        if(issig())
            goto psig;
        spl6();
        rp->p_wchan = chan;
        rp->p_stat = SWAIT;
        rp->p_pri = pri;
        spl0();
        if(runin != 0) {
            runin = 0;
            wakeup(&runin);
        }
        swtch();
        if(issig())
            goto psig;
    } else {
        spl6();
        rp->p_wchan = chan;
        rp->p_stat = SSLEEP;
        rp->p_pri = pri;
        spl0();
        swtch();
    }
    PS>integ = s;
    return;

psig:
    aretu(u.uqsav);
}