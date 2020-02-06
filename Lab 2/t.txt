#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include "h.h"

//Holds command path
char *commandPath[16];
//Holds home path for cd command
char *homePath;

//Main method
int main(int argc, char *argv[], char *env[]) {
    char *input_str;
    char **input_array;
    getPath();
    getHome();

    //Run Loop
    while(1) {
        input_str = getInput();                     //Gets input from user
        input_array = parseInput(input_str);        //Parses input by spaces into an array
        runCommand(input_array, env);               //Runs the command based on arguments
        memset(input_str, 0, strlen(input_str));    //Clears the input
        memset(input_array, 0, sizeof(input_array));//Clears the array
    }
    return 0;
}

//Gets PATH variable
void getPath() {
    int i = 0;
    char *tok;
    char *name = "PATH";                //PATH Variable
    char *tempname = getenv(name);      //Gets environment variable

    tok = strtok(tempname, ":");        //Tokenizes PATH variable based on :
    while(tok) {                        //Finishes tokenizing    
        commandPath[i] = tok;
        tok = strtok(NULL, ":");
        i++;
    }
    commandPath[i] = NULL;              //Terminates string
}

//Gets HOME variable
void getHome() {
    int i = 0;
    int j = 0;
    char *name = "HOME";                                        //Searches for home
    char *tempName = getenv(name);                              //Gets environment vaiable
    homePath = (char *)malloc(sizeof(char)*strlen(tempName));   //Allocates memory in size of the environment variable
    while(tempName[i]) {                                        //Will run through and set home path
        homePath[j] = tempName[i];
        j++;
        i++;
    }
    homePath[j] = 0;                                            //Terminates homePath
}

//Gets input
char* getInput() {
    char input[1024];
    char *temp = NULL;
    int input_length = -1;
    printf("$User: ");                                  //User prompt
    fgets(input, 1024, stdin);                          //Get input
    input_length = strlen(input);                       //Gets length of input
    temp = (char*)malloc(sizeof(char)*input_length);    //Allocates temp as the size of input
    strcpy(temp, input);                                //Copies string
    if(temp[input_length - 1] == '\n')                  //Checks and removes newline
        temp[input_length - 1] = 0;

    return temp;
}

//Parses input into an array
char** parseInput(char *in_string) {
    int i = 0; 
    char *string = NULL;                                    
    char *tempstring = NULL;
    char **returnArr = (char **)malloc(sizeof(char *)*20);
    string  = strtok(in_string, " ");                               //Tokenizes input by spaces
    while(string) {
        tempstring = (char *)malloc(sizeof(char)*strlen(string));   //Copies into a temp string, then adds to the return Array
        strcpy(tempstring, string);
        returnArr[i] = tempstring;
        string = strtok(NULL, " ");
        i++;
    }
    returnArr[i] = NULL;
    return returnArr;
}

//Runs command
void runCommand(char **inputArr, char *env[]) {
    int i = 0;
    char *temp;

    if(strcmp(inputArr[0], "exit") == 0){       //Exit command
        exit(1);
    }
    else if (strcmp(inputArr[0], "cd") == 0) {  //Cd command
        if(inputArr[1]) {
            chdir(inputArr[1]);                 //Change dir if given an argument
        }
        else {
            chdir(homePath);                    //Change dir to home if not given an argument
        }
    }
    else {
        forkChild(inputArr, env);               //Else forks a child

        while(inputArr[i]) {                    //Frees input array 
            free(inputArr[i]);
            i++;
        }
        free(inputArr);
    }
}

//Forks a child process
int forkChild(char **inputArr, char *env) {
    int i = 0;
    int status = 0;
    int pipe = 0;
    int pid;
    char **head = NULL;
    char **tail = NULL;

    head = (char **)malloc(sizeof(char*)*20);
    tail = (char **)malloc(sizeof(char*)*20);

    pid = fork();                                   //Forks process

    if(pid < 0) {                                   //Errors if fork doesn't happen
        printf("ERROR: Can't fork\n");
        exit(0);
    }
    if(pid) {                                       //Sucessful fork (pid > 0)
        //printf("Forking child, PID = %d\n", pid); //Debug statement
        pid = wait(&status);                        //Waits for command exeution
    }
    else {                                          
        pipe = getPipe(inputArr, head, tail);       //Pipes
        //printf("pipe = %d\n", pipe);              //Debug statement
        
        if(pipe) {                                  //If pipe (pipe > 0)
            i = 0;                                  //Iterates through head
            while(head[i]) {
                Pipeline(head, tail, env);          //Pipeline 
            }
        }
        else {
            runInput(inputArr, env);                //Runs the command input
        }
    }
    free(head);                                     //Frees head and tail
    free(tail);
    //printf("Child exited with status = %d\n", status);
    return status;
}

//Gets pipe
int getPipe(char **inputArr, char **head, char **tail) {
    int i = 0;
    int j = 0;
    int retVal = 0;
    char *tempStr;

    while(retVal == 0 && inputArr[i]) {         //As long as there is input and not returning
        if(strcmp(inputArr[i], "|") == 0) {     //Check for |
            retVal = 1;                 
            tempStr = inputArr[i];              //Grabs input
            inputArr[i] = NULL;
            i++;

            while(inputArr[i]) {                //Sets tail to the inputArr
                tail[j] = inputArr[i];
                i++;
                j++;
            }
            inputArr[i] = NULL;
            tail[j] = NULL;
            i = 0;
            j = 0;

            while(inputArr[i]) {                //Sets head to inputArr
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

//Pipeline command
void Pipeline(char **head, char **tail, char *env[]) {
    int pid;
    int pd[2];
    int status;

    pipe(pd);
    pid = fork();               //Forks process for the child command

    if(pid) {                   //If forked, run the command on a pipe
        close(pd[0]);
        close(1);
        dup(pd[1]);
        pid = wait(&status);
        runInput(head, env);
    }
    else {                      //If pid == 0, then do not wait and run command on new pipe
        close(pd[1]);
        close(0);
        dup(pd[0]);
        runInput(head, env);
    }
}

//Gets redirect from input Array and redirect to addr
char *getRedirect(char **inputArr, int *redir) {
    int i = 1;
    char *temp = NULL;
    char *temp2 = NULL;

    while((*redir) == 0 && inputArr[i]) {       //While the redirect isn't 0 and there is an item in the input array
        if(strcmp(inputArr[i], "<") == 0)       //Checks for < key
            *redir = 1;                         //Sets redirect to value for later use
        else if(strcmp(inputArr[i], ">") == 0)
            *redir = 2;
        else if(strcmp(inputArr[i], ">>") == 0)
            *redir = 3;

        if(*redir) {                            //If redir exits
            temp = inputArr[i+1];               //Grabs input in front of one we have
            temp2 = inputArr[i];                //Grabs current input
            inputArr[i] = NULL;                 //Nulls out
            inputArr[i+1] = NULL;
            free(temp2);                        //frees
        }
        i++;
    }
    return temp;
}

//Does the redirect
void doRedirect(char *file, int redir) {
    if(redir == 1) {                                    //redirect is 1
        close(0);                                       //closes open file
        open(file, O_RDONLY);                           //opens file as read only
    }
    else if (redir == 2) {
        close(1);
        open(file, O_WRONLY|O_CREAT, 0644);             //opens readonly and create
    }
    else if (redir == 3) {
        close(1);                   
        open(file, O_APPEND|O_WRONLY|O_CREAT, 0644);    //opens append, read only and create
    }
    else {
        printf("ERROR: Could not redirect\n");
    }
}

//Handles any command not cd or exit
void otherCommand(char **inputArr, char *env[]) {
    int i = 0;
    char cmd[128];
    char new[1024];

    strcpy(cmd, inputArr[0]);           //Copies command into cmd
    while(commandPath[i]) {             //Checks the command directories
        strcpy(new, commandPath[i]);    
        strcat(new, "/");
        strcat(new, cmd);
        execve(new, inputArr, env);
        i++;
    }
    printf("ERROR: Command %s not found\n", inputArr[0]);
}

//Runs the input
void runInput(char **inputArr, char *env[]) {
    int redir = 0;
    char *file = NULL;
    file = getRedirect(inputArr, &redir);   //Gets the redirect
    if(redir) {                             //As long as the redirect works, then do the redirect
        doRedirect(file, redir);
    }
    otherCommand(inputArr, env);            //Handle any other input
}