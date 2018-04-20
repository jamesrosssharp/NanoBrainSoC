/*=============================================================================
                                                    _
         .-==-.                   ..--/\/\--..     (_)
    __      ___    __      ____  ___  ____  ___    ____
   /  \  /\/ _ \/\/  \  /\/ _  \/ _ \/ _  \/ _ \/\/ /  \  /\
  / /\ \/ / (_)  / /\ \/ / (_) / (_)/ (_) / (_)  / / /\ \/ /
  \/  \__/\__/\_/\/  \__/\____/ (_) \ __ (\__/\_/_/\/  \__/
                              \_____//  \_\
   .-==-.           --===/\/\/\===---             ..--/\/\--..

    NanoBrain C Compiler (c) 2017 James Ross Sharp

        Test C program to test compiler

=============================================================================*/

short a = 2;
short b = 3;

/*short sum(short a, short b)
{
    return a + b;
}*/

void putch(char c)
{
    asm volatile (
                    "load r1, 0 \n"
                    "out  %0, r1 \n"
                    :
                    : "r" (c)
                    : "r1"
                );
}

short myfunc()
{
    return (((2 + 3) + (4 - 5) - 1) & 1) ^ 3;
}

void myfunc2(short a)
{
    if (a & 1)
        putch('1');
    else if (a & 2)
        putch('2');
    else
        putch('3');

    putch('\r');
    putch('\n');
}

void myfunc3(short b)
{
    while (b)
    {
        putch('A');
        b--;
    }

    putch('\r');
    putch('\n');
}

void myfunc4(short c)
{
    do
    {
        putch('B');
    }
    while (--c);

    putch('\r');
    putch('\n');
}

short main ()
{
    putch('H');
    putch('e');
    putch('l');
    putch('l');
    putch('o');
    putch('\r');
    putch('\n');

    myfunc2(myfunc());
    myfunc3(5);
    myfunc4(5);
}
