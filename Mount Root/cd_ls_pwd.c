/************* cd_ls_pwd.c file **************/

int chdir(char *pathname)
{
  printf("chdir %s\n", pathname);
  //printf("under construction READ textbook HOW TO chdir!!!!\n");
  // READ Chapter 11.7.3 HOW TO chdir
  int ino = 0;
  MINODE *mip = running->cwd;
  MINODE *newmip = NULL;

  if(!strcmp(pathname, "")) {
    running->cwd = root;
    return;
  }
  if(!strcmp(pathname, "/")) {
    running->cwd = root;
    return;
  }
  printf("Path: %s\n", pathname);
  ino = getino(pathname);
  if (ino == 0)
  {
    printf("Dir %s does not exits\n", pathname);
    return;
  }
  
  newmip = iget(dev, ino);
  if(!S_ISDIR(newmip->INODE.i_mode)) {
    printf("%s is not a directory\n", pathname);
    iput(newmip);
    return;
  }

  running->cwd = newmip;
  iput(newmip);
  return;
}

int ls(char *pathname)
{
  printf("ls %s\n", pathname);
  int ino = getino(pathname);
  MINODE *mip = iget(dev, ino);

  do_ls(pathname);

  iput(mip);
}

int do_ls(char *path) {
  u32 ino;
  MINODE *mip, *pip;
  int device = running->cwd->dev;
  char *child;

  if (path[0] == 0) {
    mip = iget(device, running->cwd->ino);
    printChild(device, mip);
  }
  else {
    if(path[0] == '/') {
      device = root->dev;
    }

    ino = getino(&device);
    if(!ino) {
      return 1;
    }

    mip = iget(device, ino);
    if(((mip->INODE.i_mode) & 0040000) != 0040000) {
      if(findparent(path)) {
        child = basename(path);
      }
      else {
        child = (char *)malloc((strlen(path) + 1) * sizeof(char));
        strcpy(child, path);
      }

      printFile(mip, child);
      return 0;
    }

    printChild(device, mip);
  }
  iput(mip);
  return 0;
}

void printFile(MINODE *mip, char *namebuf) {
  char *Time;
  u16 mode;
  int type;

  mode = mip->INODE.i_mode;
  if((mode & 0120000) == 0120000) {
    printf("l");
    type = LINK;
  }
  else if((mode & 0040000) == 0040000) {
    printf("d");
    type = DIRECTORY;
  }
  else if((mode & 0100000) == 0100000) {
    printf("-");
    type = FILE;
  }

  if((mode & (1 << 8))) {
    printf("r");
  }
  else {
    printf("-");
  }
  if((mode & (1 << 7))) {
    printf("w");
  }
  else {
    printf("-");
  }
  if((mode & (1 << 6))) {
    printf("x");
  }
  else {
    printf("-");
  }
  if((mode & (1 << 5))) {
    printf("r");
  }
  else {
    printf("-");
  }
  if((mode & (1 << 4))) {
    printf("w");
  }
  else {
    printf("-");
  }
  if((mode & (1 << 3))) {
    printf("x");
  }
  else {
    printf("-");
  }
  if((mode & (1 << 2))) {
    printf("r");
  }
  else {
    printf("-");
  }
  if((mode & (1 << 1))) {
    printf("w");
  }
  else {
    printf("-");
  }
  if((mode & 1)) {
    printf("x");
  }
  else {
    printf("-");
  }

  printf(" %d %d %d %d", mip->INODE.i_links_count, mip->INODE.i_uid, mip->INODE.i_gid, mip->INODE.i_size);
  Time = ctime(&(mip->INODE.i_mtime));
  Time[strlen(Time) - 1] = 0;
  printf(" \t%s ", Time);

  if (type == FILE) {
    printf(KNRM"");
  }
  else if (type == LINK) {
    printf(KCYN"");
  }
  else
  {
    printf(KNRM"");
  }

  printf(" \t%s ", namebuf);
  printf(KNRM"");

  if ((mode & 0120000) == 0120000) {
    printf(" => %s\n", (char *)(mip->INODE.i_block));
  }
  else {
    printf("\n");
  }

  iput(mip);
  
}

void printChild(int devicename, MINODE *mip) {
  char buf[BLKSIZE], namebuf[256], *cp;
  DIR *dirptr;
  int i, ino;
  MINODE *mip;

  for(i = 0; i <= 11; i++) {
    if(mip->INODE.i_block[i]) {
      get_block(devicename, mip->INODE.i_block[i], buf);
      cp = buf;
      dirptr = (DIR *)buf;

      while(cp < &buf[BLKSIZE]) {
        strncpy(namebuf, dirptr->name, dirptr->name_len);
        namebuf[dirptr->name_len] = 0;

        ino = dirptr->inode;
        mip = iget(devicename, ino);
        printFile(mip, namebuf);
        cp += dirptr->rec_len;
        dirptr = (DIR *)cp;
      }
    }
  }
}

int findparent(char *pn) {
  int i = 0;
  while (i < strlen(pn)) {
    if(pn[i] == '/') {
      return 1;
    }
    return 0;
  }
}

char *pwd(MINODE *wd)
{
  if(running->cwd == root) {
    printf("/");
  }
  else {
    do_pwd(running->cwd);
  }
  printf("\n");
  return 0;
}

void do_pwd(MINODE *wd) {
  struct DIR *dirptr;
  char myname[256];
  unsigned long myino, parentino;
  int f;

  if (wd == root) {
    return;
  }

  findino2(wd, &myino, &parentino);
  wd = iget(wd->dev, parentino);
  do_pwd(wd);
  findmyname(wd, myino, myname);
  printf("/%s", myname);
  iput(wd);
}

