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

short sum(short a, short b)
{
    return a + b;
}

/*void putch(char c)
{
    asm volatile (
                    " load r1, 0 \n"
                    " out  %1, r1 \n"
                    :
                    : "=r" (c)
                );


}*/

short main ()
{
/*    putch('H');
    putch('e');
    putch('l');
    putch('l');
    putch('o');

    putch('0' + sum(a,b) << 2 - 1);

    putch('\r');
    putch('\n');

    while (1)
           ;*/

    return sum(a,b);
}
