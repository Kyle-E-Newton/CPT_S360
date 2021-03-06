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

char *t1 = "xwrxwrxwr-------";
char *t2 = "----------------";

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
            if(strcmp(args[0], "-l")) {
                dirptr = opendir(".");
                if (strlen(args[1]))
                {
                    dirptr = opendir(args[1]);
                }
                while ((ent = readdir(dirptr)) != NULL)
                {
                    printf("%s\n", ent->d_name);
                }
            }
            else {
                dirptr = opendir(".");
                if (dirptr) {
                    while(ent = readdir(dirptr)) {
                        if(strcmp(ent->d_name, ".") && strcmp(ent->d_name, "..")) {
                            ls_file(ent->d_name);
                            printf("\n");
                        }
                    }
                }
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
            if (!strcmp(ok, "OK"))
            {
                fd = open(str, O_WRONLY | O_CREAT, 0755);
                total = 0;
                while (total < size)
                {
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
            n = write(server_sock, line, MAX);
            int fd, size, ln, total, r;
            struct stat mystat, *sp;
            char buff[BLK], ans[BLK];

            sp = &mystat;
            r = stat(args[0], sp);
            if (r < 0)
            {
                printf("Client: Can't See stat\n");
            }
            if (!S_ISREG(sp->st_mode))
            {
                printf("%s is not a REG file\n", args[0]);
            }
            printf("Args[0] = %s\n", args[0]);
            fd = open(args[0], O_RDONLY);
            if (fd < 0)
            {
                printf("Cannot open %s for READ\n", args[0]);
            }

            sprintf(ans, "OK %d", sp->st_size);
            printf("Ans: %s\n", ans);
            write(server_sock, ans, BLK);

            total = 0;

            while (ln = read(fd, buff, BLK))
            {
                write(server_sock, buff, ln);
                //printf("%s\n", buff);
                total += ln;
                printf("n=%d total=%d\n", ln, total);
                bzero(buff, BLK);
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

int ls_file(char *fname) {
    struct stat fstat, *sp;
    int r, i;
    char ftime[64] = "";
    char linkname[12] = "";
    sp = &fstat;
    if((r = lstat(fname, &fstat)) < 0) {
        exit(1);
    }
    if ((sp->st_mode & 0xF000) == 0x8000)   //REG
        printf("%c", '-');
    if ((sp->st_mode & 0xF000) == 0x4000)   //DIR
        printf("%c", 'd');
    if ((sp->st_mode & 0xF000) == 0xA000)   //LNK
        printf("%c", 'l');
    for(i = 8; i >= 0; i--) {
        if(sp->st_mode & (1 << i))  //print r|w|x
            printf("%c", t1[i]);
        else
            printf("%c", t2[i]);
    }
    printf("%4d ", sp->st_nlink);
    printf("%4d ", sp->st_gid);
    printf("%4d ", sp->st_uid);
    printf("%8d ", sp->st_size);
    strcpy(ftime, ctime(&sp->st_ctime));
    ftime[strlen(ftime) - 1] = 0;
    printf("%s  ", ftime);
    printf("%s ", fname);
    if((sp->st_mode & 0xF000) == 0xA000) {
        readlink(fname, linkname, 128);
        printf(" -> %s", linkname);
    }
}
