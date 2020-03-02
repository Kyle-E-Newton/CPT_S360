// The echo client client.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/socket.h>
#include <netdb.h>

#include <fcntl.h>
#include <dirent.h>

#define MAX 256

// Define variables
struct hostent *hp;
struct sockaddr_in server_addr;

int server_sock, r;
int SERVER_IP, SERVER_PORT;

char *cmd, cwd[128], *pathname, line[MAX], ans[MAX];
char args[10][MAX];

// clinet initialization code

int client_init(char *argv[])
{
    printf("======= clinet init ==========\n");

    printf("1 : get server info\n");
    hp = gethostbyname(argv[1]);
    if (hp == 0)
    {
        printf("unknown host %s\n", argv[1]);
        exit(1);
    }

    SERVER_IP = *(long *)hp->h_addr;
    SERVER_PORT = atoi(argv[2]);

    printf("2 : create a TCP socket\n");
    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock < 0)
    {
        printf("socket call failed\n");
        exit(2);
    }

    printf("3 : fill server_addr with server's IP and PORT#\n");
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = SERVER_IP;
    server_addr.sin_port = htons(SERVER_PORT);

    // Connect to server
    printf("4 : connecting to server ....\n");
    r = connect(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (r < 0)
    {
        printf("connect failed\n");
        exit(1);
    }

    printf("5 : connected OK to \007\n");
    printf("---------------------------------------------------------\n");
    printf("hostname=%s  IP=%s  PORT=%d\n",
           hp->h_name, inet_ntoa(SERVER_IP), SERVER_PORT);
    printf("---------------------------------------------------------\n");

    printf("========= init done ==========\n");
}

int getpathname()
{
    pathname = strtok(0, " ");
    if (pathname[0] == 0)
    {
        printf("No Pathname Specified");
        return 0;
    }
    return 1;
}

void ls()
{
}

main(int argc, char *argv[])
{
    int n;

    if (argc < 3)
    {
        printf("Usage : client ServerName SeverPort\n");
        exit(1);
    }

    client_init(argv);
    // sock <---> server
    printf("********  processing loop  *********\n");
    while (1)
    {
        memset(line, 0, sizeof(line));
        memset(ans, 0, sizeof(ans));
        memset(cwd, 0, sizeof(cwd));

        printf("input a line : ");
        bzero(line, MAX);        // zero out line[ ]
        fgets(line, MAX, stdin); // get a line (end with \n) from stdin

        line[strlen(line) - 1] = 0; // kill \n at end
        getcwd(cwd, MAX);           //Get client working directory
        if (line[0] == 0)           // exit if NULL line
            exit(0);
        else {
            char input[MAX];
            strcpy(input, line);

            cmd = strtok(line, " ");
            if(!strcmp(cmd, "exit")) {
                exit(1);
            }
            //pwd
            else if (!strcmp(cmd, "lpwd")) {
                printf("%s\n", cwd);
            }
            //cat
            else if (!strcmp(cmd, "lcat")) {
                if(getpathname()) {
                    int fd, i, n;
                    char buf[1024];
                    fd = open(pathname, O_RDONLY);
                    while (n = read(fd, buf, 1024)) {
                        for(i = 0; i < n; i++) {
                            if(buf[i] == '\n') {
                                putchar('\n');
                                putchar('\r');
                            }
                            else
                            {
                                putchar(buf[i]);
                            }
                            
                        }
                    }
                }
            }
            //ls
            else if (!strcmp(cmd, "lls")) {
                pathname = strtok(0, " ");
                if (strcmp(pathname, "") == 0) {
                    strcpy(pathname, cwd);
                }
                ls();
            }
            //cd
            else if (!strcmp(cmd, "lcd")) {
                if (getpathname()) {
                    chdir(pathname);
                }
            }
            //mkdir
            else if (!strcmp(cmd, "lmkdir")) {
                if (getpathname()) {
                    rmdir(pathname);
                }
            }
            //rmdir
            else if (!strcmp(cmd, "lrmdir")) {
                if (getpathname()) {
                    rmdir(pathname);
                }
            }
            //rm
            else if (!strcmp(cmd, "lrm")) {
                if (getpathname()) {
                    unlink(pathname);
                }
            }
            //Server Commands
            //pwd
            else if (!strcmp(cmd, "pwd")) {
                n = write(server_sock, input, MAX);
                printf("client: wrote n=%d bytes; line=(%s)\n", n, line);

                n = read(server_sock, ans, MAX);
                printf("client: read  n=%d bytes; echo=(%s)\n", n, ans);
            }
            //ls
            else if (!strcmp(cmd, "ls")) {
                n = write(server_sock, input, MAX);
                printf("client: wrote n=%d bytes; line=(%s)\n", n, line);

                n = read(server_sock, ans, MAX);
                printf("client: read  n=%d bytes; echo=(%s)\n", n, ans);
            }
            //cd
            else if (!strcmp(cmd, "cd")) {
                if (getpathname()) {
                    n = write(server_sock, input, MAX);
                    printf("client: wrote n=%d bytes; line=(%s)\n", n, line);

                    n = read(server_sock, ans, MAX);
                    printf("client: read  n=%d bytes; echo=(%s)\n", n, ans);
                }
            }
            //mkdir
            else if (!strcmp(cmd, "mkdir")) {
                if (getpathname()) {
                    n = write(server_sock, input, MAX);
                    printf("client: wrote n=%d bytes; line=(%s)\n", n, line);

                    n = read(server_sock, ans, MAX);
                    printf("client: read  n=%d bytes; echo=(%s)\n", n, ans);
                }
            }
            //rmdir
            else if (!strcmp(cmd, "rmdir")) {
                if (getpathname()) {
                    n = write(server_sock, input, MAX);
                    printf("client: wrote n=%d bytes; line=(%s)\n", n, line);

                    n = read(server_sock, ans, MAX);
                    printf("client: read  n=%d bytes; echo=(%s)\n", n, ans);
                }
            }
            //rm
            else if (!strcmp(cmd, "rm")) {
                if (getpathname()) {
                    n = write(server_sock, input, MAX);
                    printf("client: wrote n=%d bytes; line=(%s)\n", n, line);

                    n = read(server_sock, ans, MAX);
                    printf("client: read  n=%d bytes; echo=(%s)\n", n, ans);
                }
            }
            //get
            else if (!strcmp(cmd, "get")) {
                int filesize, n, count = 0, newFile;
                char *buff[MAX];

                n = read(server_sock, &filesize, sizeof(filesize));
                printf("File Size from server: %d\n", filesize);
                if (filesize == -1) {
                    printf("Server could not stat given file path.\n");
                }

                newFile = open(args[0])
            }
            //put
            else if (!strcmp(cmd, "put")) {
                if (getpathname()) {
                    n = write(server_sock, input, MAX);
                    printf("client: wrote n=%d bytes; line=(%s)\n", n, line);

                    n = read(server_sock, ans, MAX);
                    printf("client: read  n=%d bytes; echo=(%s)\n", n, ans);
                }
            }


        }
    }
}

int processInput(char *line)
{
    char *token;
    int i = 0;

    token = strtok(line, " ");
    strcpy(cmd, token);

    while (token = strtok(NULL, " "))
    {
        if (token && i < 10)
        {
            strcpy(args[i], token);
            i++;
        }
    }
    return 1;
}
int clearArgs(void) {
    int i = 0;
    while (i < 10) {
        bzero(args[i], 256);
        i++;
    }
    return 1;
}
