// This is the echo SERVER server.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <dirent.h>
#include <fcntl.h>

#include <sys/stat.h>
#include <sys/socket.h>
#include <netdb.h>

#define MAX 256
#define BLK 1024

// Define variables:
struct sockaddr_in server_addr, client_addr, name_addr;
struct hostent *hp;

int mysock, client_sock; // socket descriptors
int serverPort;          // server port number
int r, length, n;        // help variables

char command[MAX];
char args[10][256];

DIR *dirptr;
struct dirent *ent;

char *t1 = "xwrxwrxwr-------";
char *t2 = "----------------";

char cwd[128];

// Server initialization code:

int server_init(char *name)
{
    printf("==================== server init ======================\n");
    // get DOT name and IP address of this host

    printf("1 : get and show server host info\n");
    hp = gethostbyname(name);
    if (hp == 0)
    {
        printf("unknown host\n");
        exit(1);
    }
    printf("    hostname=%s  IP=%s\n",
           hp->h_name, inet_ntoa(*(long *)hp->h_addr));

    //  create a TCP socket by socket() syscall
    printf("2 : create a socket\n");
    mysock = socket(AF_INET, SOCK_STREAM, 0);
    if (mysock < 0)
    {
        printf("socket call failed\n");
        exit(2);
    }

    printf("3 : fill server_addr with host IP and PORT# info\n");
    // initialize the server_addr structure
    server_addr.sin_family = AF_INET;                // for TCP/IP
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY); // THIS HOST IP address
    server_addr.sin_port = 0;                        // let kernel assign port

    printf("4 : bind socket to host info\n");
    // bind syscall: bind the socket to server_addr info
    r = bind(mysock, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (r < 0)
    {
        printf("bind failed\n");
        exit(3);
    }

    printf("5 : find out Kernel assigned PORT# and show it\n");
    // find out socket port number (assigned by kernel)
    length = sizeof(name_addr);
    r = getsockname(mysock, (struct sockaddr *)&name_addr, &length);
    if (r < 0)
    {
        printf("get socketname error\n");
        exit(4);
    }

    getcwd(cwd ,128);
    printf("5.5: Set Chroor to %s\n", cwd);
    chroot(cwd);

    // show port number
    serverPort = ntohs(name_addr.sin_port); // convert to host ushort
    printf("    Port=%d\n", serverPort);

    // listen at port with a max. queue of 5 (waiting clients)
    printf("5 : server is listening ....\n");
    listen(mysock, 5);
    printf("===================== init done =======================\n");
}

main(int argc, char *argv[])
{
    char *hostname;
    char line[MAX];

    if (argc < 2)
        hostname = "localhost";
    else
        hostname = argv[1];

    server_init(hostname);

    // Try to accept a client request
    while (1)
    {
        printf("server: accepting new connection ....\n");

        // Try to accept a client connection as descriptor newsock
        length = sizeof(client_addr);
        client_sock = accept(mysock, (struct sockaddr *)&client_addr, &length);
        if (client_sock < 0)
        {
            printf("server: accept error\n");
            exit(1);
        }
        printf("server: accepted a client connection from\n");
        printf("-----------------------------------------------\n");
        printf("        IP=%s  port=%d\n", inet_ntoa(client_addr.sin_addr.s_addr),
               ntohs(client_addr.sin_port));
        printf("-----------------------------------------------\n");

        // Processing loop: newsock <----> client
        while (1)
        {
            n = read(client_sock, line, MAX);
            printf("Line: %s\n", line);
            if (n == 0)
            {
                printf("server: client died, server loops\n");
                close(client_sock);
                break;
            }

            // show the line string
            printf("server: read  n=%d bytes; line=[%s]\n", n, line);

            clearArgs();
            processInput(line);

            strcat(line, command);
            if (strlen(args[0]) > 0)
            {
                strcat(line, " | Argument: ");
                strcat(line, args[0]);
            }

            //Commands
            if (!strcmp(command, "pwd"))
            {
                char cwd[128];
                getcwd(cwd, MAX);
                printf("%s\n", cwd);
            }
            else if (!strcmp(command, "ls"))
            {
                if (strcmp(args[0], "-l"))
                {
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
                else
                {
                    dirptr = opendir(".");
                    if (dirptr)
                    {
                        while (ent = readdir(dirptr))
                        {
                            if (strcmp(ent->d_name, ".") && strcmp(ent->d_name, ".."))
                            {
                                ls_file(ent->d_name);
                                printf("\n");
                            }
                        }
                    }
                }
            }
            else if (!strcmp(command, "cd"))
            {
                chdir(args[0]);
            }
            else if (!strcmp(command, "mkdir"))
            {
                mkdir(args[0], 0755);
            }
            else if (!strcmp(command, "rmdir"))
            {
                rmdir(args[0]);
            }
            else if (!strcmp(command, "rm"))
            {
                unlink(args[0]);
            }
            else if (!strcmp(command, "quit"))
            {
                printf("Quit Recieved from Client\n");
                exit(1);
            }
            else if (!strcmp(command, "get"))
            {
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
                write(client_sock, ans, BLK);

                total = 0;

                while (ln = read(fd, buff, BLK))
                {
                    write(client_sock, buff, ln);
                    //printf("%s\n", buff);
                    total += ln;
                    printf("n=%d total=%d\n", ln, total);
                    bzero(buff, BLK);
                }
                printf("server: ready for next request\n");
            }
            else if (!strcmp(command, "put"))
            {
                printf("GET\n");
                n = write(client_sock, line, MAX);
                printf("client: wrote n=%d bytes; line=(%s)\n", n, line);

                char *str;
                str = strtok(line, " ");
                str = strtok(NULL, "");
                printf("Entering get: str = %s\n", args[0]);

                int fd, size, ln, total, r;
                char buff[BLK];
                char ok[MAX];

                ln = read(client_sock, buff, BLK);
                sscanf(buff, "%s %d", ok, &size);
                printf("buf = %s\n", buff);
                printf("ok = %s\n", ok);
                printf("Size = %d\n", size);
                if (!strcmp(ok, "OK"))
                {
                    fd = open(args[0], O_WRONLY | O_CREAT, 0755);
                    total = 0;
                    while (total < size)
                    {
                        printf("GET\n");
                        ln = read(client_sock, buff, BLK);
                        write(fd, buff, ln);
                        total += ln;
                        printf("n=%d total=%d\n", ln, total);
                    }
                    printf("server: ready for next request\n");
                }
            }

            if (strcmp(command, "get") && strcmp(command, "put"))
            {
                strcat(line, " ECHO");
                // send the echo line to client
                n = write(client_sock, line, MAX);

                printf("server: wrote n=%d bytes; ECHO=[%s]\n", n, line);
                printf("server: ready for next request\n");
            }
        }
    }
}

int processInput(char *line)
{
    char *token;
    int i = 0;

    token = strtok(line, " ");
    strcpy(command, token);

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