/************* t.c file ********************/
#include <stdio.h>
#include <stdlib.h>

int *FP;

int main(int argc, char *argv[], char *env[])
{
    int a, b, c;
    printf("enter main\n");

    printf("&argc=%x argv=%x env=%x\n", &argc, argv, env);
    printf("&a=%8x &b=%8x &c=%8x\n", &a, &b, &c);

    //(1). Write C code to print values of argc and argv[] entries

    a = 1;
    b = 2;
    c = 3;
    A(a, b);
    printf("exit main\n");
}

int A(int x, int y)
{
    int d, e, f;
    printf("enter A\n");
    // write C code to PRINT ADDRESS OF d, e, f
    d = 4;
    e = 5;
    f = 6;
    printf("&d=%d &e=%d &f=%d", &d, &e, &f);
    B(d, e);
    printf("exit A\n");
}

int B(int x, int y)
{
    int g, h, i;
    printf("enter B\n");
    // write C code to PRINT ADDRESS OF g,h,i
    g = 7;
    h = 8;
    i = 9;
    printf("&g=%d &h=%d &i=%d", &g, &h, &i);
    C(g, h);
    printf("exit B\n");
}

int C(int x, int y)
{
    int u, v, w, i, *p;

    printf("enter C\n");
    // write C cdoe to PRINT ADDRESS OF u,v,w,i,p;
    u = 10;
    v = 11;
    w = 12;
    i = 13;
    printf("&u=%d &v=%d &w=%d &i=%d &p=%d", &u, &v, &w, &i, &p);
    FP = (int *)getebp(); //FP = stack frame pointer of the C() function

    //(2). Write C code to print the stack frame link list.
    p = FP;
    while(p != 0) {
        printf("FP -> %8X -> \n", p);
        p = *p;
    }
    printf("P -> %8X\n", p);

    p = (int *)&p;

    // (3).Print the stack contents from p to the frame of main()
    //     YOU MAY JUST PRINT 128 entries of the stack contents.

    // (4).On a hard copy of the print out, identify the stack contents
    //     as LOCAL VARIABLES, PARAMETERS, stack frame pointer of each function.


}
