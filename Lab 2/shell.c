#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

char gpath[128];
char *dir[128];
int n;

int main(int argc, char *argv[], char *env[]) {
    //Print PATH
    printf("argc=%d", argc);
    int i = 0;
    while(argv[i]) {
        printf("argv[%d]=%s", i, argv[i]);
        i++;
    }
    i = 0;
    while(env[i]) {
        printf("env[%d]=%s", i, env[i]);
    }
    return 0;
}