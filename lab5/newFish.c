#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

#define RANKS 13
#define SUITS 4
#define BUFFERSIZE 156

pid_t getpid(void);
int seed;

void initrand(int myid) {
    seed = myid - 1;
}

int nextrand() {
    return (seed++ % 99) + 1;
}

void readDeck(char (*deck)[3]) {
    char inbuf[BUFFERSIZE];
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
            k++;
            while(i < n && (inbuf[i] == ' ' || inbuf[i] == '\n')) i++;
        }
    }
}



void startGame(const int N_CHILD, char (*deck)[3]) {
    int pid, i;
    int toParent[2];
    int toChild[2];
    char inbuf[BUFFERSIZE];
    pid_t shut_down[N_CHILD];
    int playerTurn;
    
    // Create pipe
    if(pipe(toParent) < 0 || pipe(toChild)) {
        printf("Pipe creation error\n");
        exit(1);
    }
    
    // Fork
    for(i = 0; i < N_CHILD; i++) {
        pid = fork();
        if(pid < 0) {
            printf("Error");
            exit(1);
        } 
        else if (pid == 0) { /* child */
            close(toChild[1]);
            close(toParent[0]);
            // Main functions
            // TODO: Initialize hand to child
            
            // Reduce function
            // Ask card function (Go fish)


            printf("Child %d, pid %d: no task\n", i+1, getpid());
            exit(0);
        }
        else {
            shut_down[i] = pid;
        }
    }

    if(pid > 0) { /* parent */
        int loop = 1;
        close(toChild[0]);
        close(toParent[1]);

        printf("Parent: the child players are ");
        for(i = 0; i < N_CHILD; i++){
            printf("%d ", shut_down[i]);
        }
        printf("\n");

        while(loop) {
            // Add parent control
            // For Debug
            loop = 0;
            printf("Parent exit loop. PID: %d\n", getpid());
        }
        close(toChild[1]);
        close(toParent[0]);
    }

    /* prevent zombie */
    for(i = 0; i < N_CHILD; i++)
        waitpid(shut_down[i], NULL, 0);
        
}


int main(int argc, char *argv[]) {
    char deck[52][3];
    int nChild = atoi(argv[1]);

    if(nChild < 2 || nChild > 8) {
        printf("Player range should be 2 to 8...\n");
        return 0;
    }
    readDeck(deck);
    startGame(nChild, deck);
    return 0;
}