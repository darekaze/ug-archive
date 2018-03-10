#include <stdio.h>
#include <unistd.h>

#define BUFFERSIZE 100

int main(int argc, char *argv[]) {
    char buffer[BUFFERSIZE];
    read(STDIN_FILENO, buffer, BUFFERSIZE);
    // fgets(buffer, BUFFERSIZE, stdin);
    printf("Read: %s", buffer);
    return 0;
}