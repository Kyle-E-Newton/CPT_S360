#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include "h.h"

char *commandPath[16];
char *homePath;

int main(int argc, char *argv[], char *env[]) {
    char *input_str;
    char **input_array;
    int i = 0;

    getPath();
    getHome();

    while(1) {
        input_str = getInput();

        printf("Input was: %s\n", input_str);

        input_array = parseInput(input_str);
        i=0;
        runCommand(input_array, env);
        printf("\n\n");
        memset(input_str, 0, strlen(input_str));
        memset(input_array, 0, sizeof(input_array));
    }
    return 0;
}

void getPath() {
    int i = 0;
    char *tok;
    char *name = "PATH";
    char *tempname = getenv(name);

    tok = strtok(tempname, ":");
    while(tok) {
        commandPath[i] = tok;
        tok = strtok(NULL, ":");
        i++;
    }
    commandPath[i] = NULL;
}

void getHome() {
    int i = 0;
    int j = 0;
    char *name = "HOME";
    char *tempName = getenv(name);
    homePath = (char *)malloc(sizeof(char)*strlen(tempName));
    while(tempName[i]) {
        homePath[j] = tempName[i];
        j++;
        i++;
    }
    homePath[j] = 0;
}

char* getInput() {
    char input[1024];
    char *temp = NULL;
    int input_length = -1;
    printf("$User: ");
    fgets(input, 1024, stdin);
    input_length = strlen(input);
    temp = (char*)malloc(sizeof(char)*input_length);
    strcpy(temp, input);
    if(temp[input_length - 1] == '\n')
    temp[input_length - 1] = 0;

    return temp;
}

char** parseInput(char *in_string) {
    int i = 0; 
    char *string = NULL;
    char *tempstring = NULL;
    char **returnArr = (char **)malloc(sizeof(char *)*20);
    string  = strtok(in_string, " ");
    while(string) {
        tempstring = (char *)malloc(sizeof(char)*strlen(string));
        strcpy(tempstring, string);
        returnArr[i] = tempstring;
        string = strtok(NULL, " ");
        i++;
    }
    returnArr[i] = NULL;
    return returnArr;
}

void runCommand(char **inputArr, char *env[]) {
    int i = 0;
    char *temp;

    if(strcmp(inputArr[0], "exit") == 0){
        exit(1);
    }
    else if (strcmp(inputArr[0], "cd") == 0) {
        if(inputArr[1]) {
            chdir(inputArr[1]);
        }
        else {
            chdir(homePath);
        }
    }
    else {
        forkChild(inputArr, env);

        while(inputArr[i]) {
            free(inputArr[i]);
            i++;
        }
        free(inputArr);
    }
}

int forkChild(char **inputArr, char *env) {
    int i = 0;
    int status = 0;
    int pipe = 0;
    int pid;
    char **head = NULL;
    char **tail = NULL;

    head = (char **)malloc(sizeof(char*)*20);
    tail = (char **)malloc(sizeof(char*)*20);

    pid = fork();

    if(pid < 0) {
        printf("ERROR: Can't fork\n");
        exit(0);
    }
    if(pid) {
        printf("Forking child, PID = %d\n", pid);
        pid = wait(&status);
    }
    else {
        pipe = getPipe(inputArr, head, tail);
        printf("pipe = %d\n", pipe);
        
        if(pipe) {
            i = 0;
            while(head[i]) {
                Pipeline(head, tail, env);
            }
        }
        else {
            runInput(inputArr, env);
        }
    }
    free(head);
    free(tail);
    printf("Child exited with status = %d\n", status);
    return status;
}

int getPipe(char **inputArr, char **head, char **tail) {
    int i = 0;
    int j = 0;
    int retVal = 0;
    char *tempStr;

    while(retVal == 0 && inputArr[i]) {
        if(strcmp(inputArr[i], "|") == 0) {
            retVal = 1;
            tempStr = inputArr[i];
            inputArr[i] = NULL;
            i++;

            while(inputArr[i]) {
                tail[j] = inputArr[i];
                i++;
                j++;
            }
            inputArr[i] = NULL;
            tail[j] = NULL;
            i = 0;
            j = 0;

            while(inputArr[i]) {
                head[j] = inputArr[i];
                i++;
                j++;
            }
            head[j] = NULL;
        }
        i++;
    }
    return retVal;
}

void Pipeline(char **head, char **tail, char *env[]) {
    int pid;
    int pd[2];
    int status;

    pipe(pd);
    pid = fork();

    if(pid) {
        close(pd[0]);
        close(1);
        dup(pd[1]);
        pid = wait(&status);
        runInput(head, env);
    }
    else {
        close(pd[1]);
        close(0);
        dup(pd[0]);
        runInput(head, env);
    }
}

char *getRedirect(char **inputArr, int *redir) {
    int i = 1;
    char *temp = NULL;
    char *temp2 = NULL;

    while((*redir) == 0 && inputArr[i]) {
        if(strcmp(inputArr[i], "<") == 0) 
            *redir = 1;
        else if(strcmp(inputArr[i], ">") == 0)
            *redir = 2;
        else if(strcmp(inputArr[i], ">>") == 0)
            *redir = 3;

        if(*redir) {
            temp = inputArr[i+1];
            temp2 = inputArr[i];
            inputArr[i] = NULL;
            inputArr[i+1] = NULL;
            free(temp2);
        }
        i++;
    }
    return temp;
}

void doRedirect(char *file, int redir) {
    if(redir == 1) {
        close(0);
        open(file, O_RDONLY);
    }
    else if (redir == 2) {
        close(1);
        open(file, O_WRONLY|O_CREAT, 0644);
    }
    else if (redir == 3) {
        close(1);
        open(file, O_APPEND|O_WRONLY|O_CREAT, 0644);
    }
    else {
        printf("ERROR: Could not redirect\n");
    }
}

void otherCommand(char **inputArr, char *env[]) {
    int i = 0;
    char cmd[128];
    char new[1024];

    strcpy(cmd, inputArr[0]);
    while(commandPath[i]) {
        strcpy(new, commandPath[i]);
        strcat(new, "/");
        strcat(new, cmd);
        execve(new, inputArr, env);
        i++;
    }
    printf("ERROR: Command %s not found\n", inputArr[0]);
}

void runInput(char **inputArr, char *env[]) {
    int redir = 0;
    char *file = NULL;
    file = getRedirect(inputArr, &redir);
    if(redir) {
        doRedirect(file, redir);
    }
    otherCommand(inputArr, env);
}