#include <stdio.h>
#include <unistd.h>

#define BUFFERSIZE 100

int main(int argc, char *argv[]) {
    char buffer[BUFFERSIZE];
    char ch;

    read(STDIN_FILENO, buffer, BUFFERSIZE);
    // fgets(buffer, BUFFERSIZE, stdin);
    // while(read(STDIN_FILENO, &ch, 1) > 0) {
    //      /* Do sth.*/ 
    // }
    printf("Read: %s\n", buffer);
    return 0;
}