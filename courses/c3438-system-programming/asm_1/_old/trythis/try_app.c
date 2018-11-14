#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MYADC "/dev/adc_driver"

void Delay_MS( unsigned int time) { // 50 ns
    unsigned int i,j;
    
    for ( i=0; i<time; i++) {
        for(j=0;j<30000;j++) {
        } 
    } 
}
//adc可调电阻
int main(void) {
    int fd;
    int i=0;
    unsigned int value = -1;
    char buf[30]={0};

    fd = open(MYADC,O_RDWR,0666);
    if (fd < 0) {
        perror("open device adc_driver error\n");
        exit(1);
    }
    printf("open /dev/adc_driver success!\n");

    while(1) {
        read(fd,&value,4);
        printf("result value=%d\n",value);
        Delay_MS(1000);
    }
    
    if(close(fd)<0) {
        perror("close error\n");
        exit(1);
    }
    return 0;
}