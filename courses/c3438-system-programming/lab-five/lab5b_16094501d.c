#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


int main() {
  int fd;
  char zero = '0', one = '1';

  char s1[] = "/dev/lab5b_1";
  int len = sizeof(s1)/sizeof(char);

  do {
    int i;
    for(i = 1; i < 5; i++) {
      s1[len-2] = '0' + i;
      fd = open(s1, O_WRONLY);
      if(fd == -1) {
        printf("Fail to open device lab5b_1!\n");
        goto finish;
      }
      write(fd, &zero, 1);
      close(fd);
    }
    for(i = 1; i < 5; i++) {
      s1[len-2] = '0' + i;
      fd = open(s1, O_WRONLY);
      if(fd == -1) {
        printf("Fail to open device lab5b_1!\n");
        goto finish;
      }
      write(fd, &one, 1);
      sleep(1);
      close(fd);
    }
    for(i = 4; i > 0; i--) {
      s1[len-2] = '0' + i;
      fd = open(s1, O_WRONLY);
      if(fd == -1) {
        printf("Fail to open device lab5b_1!\n");
        goto finish;
      }
      write(fd, &zero, 1);
      sleep(1);
      close(fd);
    }
  } while(1);

finish:
  return 0;
}
