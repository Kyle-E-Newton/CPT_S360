#include <stdio.h>
#include <stdlib.h>

typedef unsigned int u32;

char *ctable = "0123456789ABCDEF";
int BASE = 10;
int *FP;

int rpu(u32 x) {
    char c;
    if(x) {
        c = ctable[x % BASE];
        rpu(x / BASE);
        putchar(c);
    }
}

int printu(u32 x) {
    (x==0)? putchar('0') : rpu(x);
    putchar(' ');
}

int printd(int x) {
    if (x < 0) {
        x = x*-1;
        prints("-");
    }
    char c;
    if (x) {
        c = ctable[x % BASE];
        printd(x / BASE);
        putchar(c);
    }
}

void prints(char *s) {
    while(*s) {
        putchar(*s);
        s++;
    }
}

int printx(u32 x) {
    prints("0x");
    hex(x);
}

void hex(u32 x) {
    char c;
    if (x) {
        c = ctable[x % 16];
        hex(x / 16);
        putchar(c);
    }
}

int printo(u32 x) {
    prints("0o");
    oct(x);
}

void oct(u32 x) {
    char c;
    if (x) {
        c = ctable[x % 8];
        oct(x / 8);
        putchar(c);
    }
}

myprintf(char *fmt, ...) {
    char *cp;
    int *ip;
    asm("movl %ebp, FP");
    ip = FP + 3;

    for(cp = fmt; *cp != '\0'; cp++) {
        if(*cp != '%') {
            putchar(*cp);
            if(*cp == '\n') {
                putchar('\r');
            }
        }
        if(*cp == '%') {
            cp++;
            switch(*cp) {
                case 'c': putchar((char)(*ip));
                        break;
                case 's': prints((char*)(*ip));
                        break;
                case 'u': printu(*ip);
                        break;
                case 'd': printd(*ip);
                        break;
                case 'o': printo(*ip);
                        break;
                case 'x': printx(*ip);
            }
            ip++;
        }
    }

}

int main(int argc, char* argv[], char *env[]) {
    //Test Functions
    myprintf("THIS IS A TEST\n");
    myprintf("cha=%c, string=%s, dec=%d, hex=%x, oct=%o, neg=%d\n", 'Z', "Testing this", 100, 100, 100, -100);
}

