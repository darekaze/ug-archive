#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

pid_t getpid(void);

int val = 2;

int main(int argc, char *argv[]) {
    int pid;
    
    for(int i = 0; i < atoi(argv[1]); i++) {
        pid = fork();
        if(pid < 0) {
            printf("Error");
            exit(1);
        } else if (pid == 0) {
            // Child do sth
            val++;
            printf("Child (%d): %d, %d\n", i+1, getpid(),val);
            exit(0);
        }
    }
    int stat;
    for(int i = 0; i < atoi(argv[1]); i++) {
        wait(&stat);
        int a = getpid();
        printf("%d\n",a);
    }
}

