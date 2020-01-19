/********** h2.c *********/
#include <stdio.h>
#include <string.h>

#include <fcntl.h>

typedef struct person{
  char name[64];
  int  id;
  int  age;
  char gender;
}PERSON;

int write_sector(int fd, int sector, char *buf)
{
  int n;
  lseek(fd, sector*512, SEEK_SET); // advance to sector*512 bytes
  n = write(fd, buf, 512);         // write 512 bytes from buf[] to sector
  if (n != 512){
     printf("write failed\n");
     return -1;
  }
  return n;
}

PERSON kcw, *p;

int fd;
char buf[512];

int main()
{
  p = &kcw;
	  
  strcpy(p->name, "k.c. Wang");
  p->id = 12345678;
  p->age = 83;
  p->gender = 'M';
  
  fd = open("disk", O_WRONLY);  // open disk file for WRITE
  printf("fd = %d\n", fd);      // show file descriptor number

  memcpy(buf+256, p, sizeof(PERSON)); // copy *p to byte 256 in buf[ ]
  write_sector(fd, 2, buf);     // write buf[512] to sector 2
}