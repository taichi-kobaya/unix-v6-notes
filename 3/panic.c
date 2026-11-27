panic(s)
char *s;
{
    panicstr = s;
    update();
    printf("panic: %s\n", s);
    for(;;)
        idle();
}