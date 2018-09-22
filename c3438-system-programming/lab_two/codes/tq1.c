#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    int i;
    pid_t childPid;

    pid_t myPid = getpid();
    for(i = 0; i < 4; i++) {
        printf("i %d - ", i);
        childPid = fork();
        while(childPid == -1) {
            childPid = fork();
        }
        if(childPid == 0) break;
    }

    if(myPid == getpid()) {
        int state;
        while(wait(&state) > 0);
    }

    printf("my pid is %d, my ppid is %d\n", getpid(), getppid());
    return 0;
}
