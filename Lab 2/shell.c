#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

char gpath[128];
char *dir[128];
char *cmds[128];
int n;

int main(int argc, char *argv[], char *env[]) {
    char *s;
    //Print PATH
    printf("argc=%d\n", argc);
    int i = 0;
    while(argv[i]) {
        printf("argv[%d]=%s\n", i, argv[i]);
        i++;
    }
    i = 0;
    while(env[i]) {
        if(strncmp(env[i], "PATH=", 5) == 0) {
            printf("Found PATH: %s\n", env[i]);
            strcpy(gpath, &env[i][5]);
            printf("gpath=%s\n", gpath);
            break;
        }
        i++;
    }

    n = 0;
    s = strtok(gpath, ":");

    while(s) {
        dir[n++] = s;
        s = strtok(NULL, ":");
    }

    for( i = 0; i < n; i++) {
        printf("dir[%d] = %s\n", i, dir[i]);
    }

    char *c;
    while(1) {
        char str[128] = {};
        memset(str, '\0', 128);
        n = 0;
        printf("User: ");
        gets(str);
        c = strtok(str, " ");
        while(c) {
            cmds[n++] = c;
            c = strtok(NULL, " ");
        }
        handleCommand(cmds);
    }
    return 0;
}

void handleCommand(char * cmd[]) {
    char* c;
    strcpy(c, cmd[0]);
    switch(c) {
        case 'cd':
            printf("CD\n");
            break;
        case 'ls':
            printf("LS\n");
            break;
        default:
            printf("OTHER\n");
            break;
    }
}