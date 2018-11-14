#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <linux/fs.h>
#include <errno.h>
#include <string.h>

int main(void)
{
        fprintf(stderr, "press Ctrl-C to stop\n");
        int fd = open("/dev/myadc", "r");
        if (fd < 0) {
                perror("open ADC device:");
                return 1;
        }
        ioctl(fd,'s',0);
        int ADCValue=0;
        double voltage=0;
        for(;;) {
                ADCValue=0;
                voltage=0;
                read(fd, &ADCValue, sizeof(int));
                if(ADCValue==-1) continue;
                //channel 0 12bit ADC max voltage = 3.3v /10K+1k *10K =3v
                voltage=(float)3.3*ADCValue/4096;
                printf("adc = %d voltage=%f V \n",ADCValue,voltage);
                usleep(500* 1000);
        }

        close(fd);
}