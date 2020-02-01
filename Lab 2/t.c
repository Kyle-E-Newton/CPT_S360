#include <stdio.h>
#include <string.h>

char gpath[128];
char *dir[128];
int n;

int main(int argc, char *argv[], char *env[]) {
    char *s;
    int i = 0;

    printf("argc = %d\n", argc);

    while(argv[i]) {
        printf("argv[%d] = %s\n", i, argv[i]);
        i++;
    }

    getchar();

    printf("Looking for the PATH variable\n");
    i = 0;
    while(env[i]) {
        if(strncmp(env[i], "PATH=", 5) == 0) {
            printf("found PATH : %s\n", i, env[i]);
            printf("Copy PATH string to gpath[]\n");
            strcpy(gpath, &env[i][5]);
            printf("gpath=%s\n", gpath);
            break;
        }
        i++;
    }

    printf("Tokenize gpath[] into token strings\n");
    n = 0;
    s = strtok(gpath, ":");

    while(s) {
        dir[n++] = s;
        s = strtok(NULL, ":");
    }

    printf("n=%d\n", n);

    for(i = 0; i < n; i++) {
        printf("dir[%d] = %s\n", i, dir[i]);
    }
}