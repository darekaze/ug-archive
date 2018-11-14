#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <sys/ioctl.h>

#define ADC_SET_CHANNEL         0xc000fa01
#define ADC_SET_ADCTSC          0xc000fa02

#define CHANNELCOUNT 6

/*
 * This file is modified from week 10 code.zip
 */

int main(int argc, char* argv[]) {
  int channels[CHANNELCOUNT] = {0,1,6,7,8,9};
  int channel;
  int i = 0;
  
  fprintf(stderr, "press Ctrl-C to stop\n");
  int fd = open("/dev/jahja_adc", 0);
  if (fd < 0) {
    perror("open ADC device:");
    return 1;
  }

  char output[255];
  while(1) {
    puts("\033[2J");
    output[0] = 0;
    for (i=0; i<CHANNELCOUNT; i++) {
      channel = channels[i];
      if (ioctl(fd, ADC_SET_CHANNEL, channel) < 0) {
        perror("Can't set channel for /dev/jahja_adc!");
        close(fd);
        return 1;
      }

      char buffer[30];
      int len = read(fd, buffer, sizeof buffer -1);
      if (len > 0) {
        buffer[len] = '\0';
        int value = -1;
        sscanf(buffer, "%d", &value);
        char outbuff[255];
        sprintf(outbuff, "AIN%d %d\n", channel, value);
        strcat(output, outbuff);
      } else {
        perror("Read ADC device:");
        close(fd);
        return 1;
      }
    }
    printf("%s",output);
    usleep(300 * 1000);
  }
  close(fd);
}
