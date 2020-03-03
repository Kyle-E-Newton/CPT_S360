// The echo client client.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/socket.h>
#include <netdb.h>

#include <fcntl.h>
#include <dirent.h>
#include <sys/stat.h>

#define MAX 256
#define BLK 1024

// Define variables
struct hostent *hp;
struct sockaddr_in server_addr;

int server_sock, r;
int SERVER_IP, SERVER_PORT;

char cmd[MAX], cwd[128], *pathname, line[MAX], ans[MAX];
char args[10][MAX];

int cfd;

DIR *dirptr;
struct dirent *ent;

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

main(int argc, char *argv[])
{
    int n;
    cfd = socket(AF_INET, SOCK_STREAM, 0);
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
        
        char input[MAX];
        strcpy(input, line);
        clearArgs();
        processInput(input);

        printf("Input: %s\n", line);

        //cat
        if (!strcmp(cmd, "lcat"))
        {
            if (strlen(args[0]))
            {
                int fd, i, n;
                char buf[1024];
                fd = open(args[0], O_RDONLY);
                while (n = read(fd, buf, 1024))
                {
                    for (i = 0; i < n; i++)
                    {
                        if (buf[i] == '\n')
                        {
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
        //pwd
        else if (!strcmp(cmd, "lpwd"))
        {
            printf("%s\n", cwd);
        }
        //ls
        else if (!strcmp(cmd, "lls"))
        {
            dirptr = opendir(".");
                if(strlen(args[0])) {
                    dirptr = opendir(args[0]);
                }
                while ((ent = readdir(dirptr)) != NULL) {
                    printf("%s\n", ent->d_name);
                }
        }
        //cd
        else if (!strcmp(cmd, "lcd"))
        {
            chdir(args[0]);
            
        }
        //mkdir
        else if (!strcmp(cmd, "lmkdir"))
        {
            if (strlen(args[0]))
            {
                mkdir(args[0], 0644);
            }
        }
        //rmdir
        else if (!strcmp(cmd, "lrmdir"))
        {
            if (strlen(args[0]))
            {
                rmdir(args[0]);
            }
        }
        //rm
        else if (!strcmp(cmd, "lrm"))
        {
            if (strlen(args[0]))
            {
                unlink(args[0]);
            }
        }
        //Server Commands
        //pwd/ls/cd/mkdir/rmdir/rm
        else if (!strcmp(cmd, "pwd") || !strcmp(cmd, "ls") || !strcmp(cmd, "cd") || !strcmp(cmd, "mkdir") || !strcmp(cmd, "rmdir") || !strcmp(cmd, "rm"))
        {
            n = write(server_sock, line, MAX);
            printf("client: wrote n=%d bytes; line=(%s)\n", n, line);

            n = read(server_sock, ans, MAX);
            printf("client: read  n=%d bytes; echo=(%s)\n", n, ans);
        }
        //get
        else if (!strcmp(cmd, "get"))
        {
            printf("GET\n");
            n = write(server_sock, line, MAX);
            printf("client: wrote n=%d bytes; line=(%s)\n", n, line);

            char *str;
            str = strtok(line, " ");
            str = strtok(NULL, "");
            printf("Entering get: str = %s\n", args[0]);

            int fd, size, ln, total, r;
            char buff[BLK];
            char ok[MAX];

            ln = read(server_sock, buff, BLK);
            sscanf(buff, "%s %d", ok, &size);
            printf("buf = %s\n", buff);
            printf("ok = %s\n", ok);
            printf("Size = %d\n", size);
            if(!strcmp(ok, "OK")) {
                fd = open(str, O_WRONLY|O_CREAT, 0755);
                total = 0;
                while (total < size) {
                    printf("GET\n");
                    ln = read(server_sock, buff, BLK);
                    write(fd, buff, ln);
                    total += ln;
                    printf("n=%d total=%d\n", ln, total);
                }
                close(fd);
            }
        }
        //put
        else if (!strcmp(cmd, "put"))
        {
            char *str;
            write(cfd, args[0], MAX);
            str = strtok(args[0], " ");
            str = strtok(NULL, "");

            int fd, size, ln, total, r;
            struct stat mystat, *sp;
            char buff[BLK], and[BLK];

            sp = &mystat;
            r = stat(str, sp);
            if (r < 0) {
                printf("Client: Can't Stat %s\n", str);
            }
            if(!S_ISREG(sp->st_mode)) {
                printf("%s is not REG file\n", str);
            }

            fd = open(str, O_RDONLY);
            if (fd < 0) {
                printf("Cannot open %s for Read\n", str);
            }

            sprintf(ans, "OK %d", sp->st_size);
            write(cfd, ans, BLK);

            total = 0;

            while (ln=read(fd, buff, BLK)) {
                write(cfd, buff, ln);
                total += ln;
                bzero(buff, BLK);
            }
            printf("Sent %d bytes\n",  total);
        }
        //quit
        else if (!strcmp(cmd, "quit")) {
            printf("Exiting\n");
            exit(1);
        }
        else {
            printf("Unknown Command\n");
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
int clearArgs(void)
{
    int i = 0;
    while (i < 10)
    {
        bzero(args[i], 256);
        i++;
    }
    return 1;
}
