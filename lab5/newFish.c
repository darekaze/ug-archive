#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

pid_t getpid(void);

#define RANKS 13
#define SUITS 4
#define BUFFERSIZE 156

int seed;

void initrand(int myid) {
    seed=myid-1;
}

int nextrand() {
    return (seed++ %99) + 1;
}

void startGame(int num, char deck[][3]) {
    int pid, i;
    int childToParent[2];
    int parentToChild[2];

    // Create pipe
    if(pipe(childToParent) < 0 || pipe(parentToChild)) {
            printf("Pipe creation error\n");
            exit(1);
    }
    
    // Fork
    for(i = 0; i < num; i++) {
        pid = fork();
        if(pid < 0) {
            printf("Error");
            exit(1);
        } 
        else if (pid == 0) { /* child */
            close(parentToChild[1]);
            close(childToParent[0]);
            // Main functions
            // TODO: Initialize hand to child
            // Reduce function
            // Ask card function (Go fish)


            printf("Child %d, pid %d: no task\n", i+1, getpid());
            exit(0);
        }
    }

    if(pid > 0) { /* parent */
        int loop = 1;
        close(parentToChild[0]);
        close(childToParent[1]);

        while(loop) {
            // Add parent control

            // For Debug
            loop = 0;
            printf("Parent exit loop. PID: %d\n", getpid());
        }
        close(parentToChild[1]);
        close(childToParent[0]);
    }

    /* prevent zombie */
    int stat;
    for(i = 0; i < num; i++)
        wait(&stat);
}


int main(int argc, char *argv[]) {
    char inbuf[BUFFERSIZE];
    const int NUM_CHILD = atoi(argv[1]);
    char deck[52][3];
    int i, k, n;

    // Read < .txt file
    while((n = read(STDIN_FILENO, inbuf, BUFFERSIZE)) > 0) {
        i = 0, k = 0;
        inbuf[n] = '\0';
        while(i < n && inbuf[i] == ' ') i++;
        while (i < n) {
            deck[k][2] = '\0';
            deck[k][0] = inbuf[i++];
            deck[k][1] = inbuf[i++];
            // printf("%s\n", deck[k]);
            k++;
            while(i < n && (inbuf[i] == ' ' || inbuf[i] == '\n')) i++;
        }
    }

    startGame(NUM_CHILD, deck);
    return 0;
}