#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

#define BIG_BUF 156
#define SMALL_BUF 6

pid_t getpid(void);
int seed;

void initrand(int myid) {
    seed = myid - 1;
}

int nextrand() {
    return (seed++ % 99) + 1;
}

void readDeck(char (*deck)[3]) {
    char inbuf[BIG_BUF];
    int i, k, n;

    // Read < .txt file
    while((n = read(STDIN_FILENO, inbuf, BIG_BUF)) > 0) {
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

int suitRank(char c) {
    int r;
    switch (c) {
        case 'd': case 'D': r = 0; break;
        case 'c': case 'C': r = 1; break;
        case 'h': case 'H': r = 2; break;
        case 's': case 'S': r = 3; break;
        default:            break;
    }
    return r;
}

int cardRank(char c) {
    int r;
    switch (c) {
        case '2':           r = 0; break;
        case '3':           r = 1; break;
        case '4':           r = 2; break;
        case '5':           r = 3; break;
        case '6':           r = 4; break;
        case '7':           r = 5; break;
        case '8':           r = 6; break;
        case '9':           r = 7; break;
        case 't': case 'T': r = 8; break;
        case 'j': case 'J': r = 9; break;
        case 'q': case 'Q': r = 10; break;
        case 'k': case 'K': r = 11; break;
        case 'a': case 'A': r = 12; break;
        default:            break;
    }
    return r;
}

void initHand(int id, int *child, int num, char (*hand)[3]) {
    int i;
    char buf[SMALL_BUF];

    for(i = 0; i < num; i++) {
        read(child[0], buf, SMALL_BUF);
        strcpy(hand[i], buf);
    }
    // sprintf for hands

    for(i = 0; i < sizeof(hand)/sizeof(*hand[0]); i++) {
        printf("%s", hand[i]);
    }
    
    printf("Child %d, pid %d: initial hand <%s> \n", id, getpid(), hand[3]);
}

void rdcHand(char (*hand)[3], char (*reduced)[3]) {

}

void startGame(const int N_CHILD) {
    int pid, i, j, loop = 1;
    int toParent[N_CHILD][2];
    int toChild[N_CHILD][2];
    char cmdBuf[SMALL_BUF];
    int nCard = (N_CHILD < 5) ? 7 : 5;
    pid_t shut_down[N_CHILD];
    char hand[52][3] = {0}, reduced[52][3] = {0};

    // Create pipe
    for(i = 0; i < N_CHILD; i++) {
        if(pipe(toParent[i]) < 0 || pipe(toChild[i]) < 0) {
            printf("Pipe creation error\n");
            exit(1);
        }
    }
    // Fork
    for(i = 0; i < N_CHILD; i++) {
        pid = fork();
        if(pid < 0) {
            printf("Error");
            exit(1);
        } 
        else if (pid == 0) { /* child */
            // usable pipe-> read: toChild[i][0] write: toParent[i][1]
            for(j = 0; j < N_CHILD; j++){
                close(toChild[j][1]);
                close(toParent[j][0]);
                if(j != i) {
                    close(toChild[j][0]);
                    close(toParent[j][1]);
                }
            }

            initHand(i+1, toChild[i], nCard, hand); // Get initial card
            rdcHand(hand, reduced);

            while(loop) {
                // Main functions -- need while loop here
                // TODO: Ask card function (Go fish)
                // checkReduce after getting card
                // Check if deck is empty
                // TODO: check hand is empty, which stops the child process
                // First one with most pair wins
                loop = 0;
            }
            

            // printf("Child %d, pid %d: fin\n", i+1, getpid());

            // Finish child process
            close(toChild[i][0]);
            close(toParent[i][1]);
            exit(0);
        }
        else {
            shut_down[i] = pid;
        }
    }

    if(pid > 0) { /* parent */
        char deck[52][3];

        // close useless pipe
        for(i = 0; i < N_CHILD; i++) {
            close(toChild[i][0]);
            close(toParent[i][1]);
        }
        
        printf("Parent: the child players are ");
        for(i = 0; i < N_CHILD; i++)
            printf("%d ", shut_down[i]);
        printf("\n");

        // usable pipe-> read: toParent[i][0] write: toChild[i][1]
        readDeck(deck);
        // Initialize player hand
        for(i = 1; i <= nCard; i++)
            for(j = 1; j <= N_CHILD; j++)
                write(toChild[j-1][1], deck[(i*j)-1], SMALL_BUF);
                

        // Game loop
        while(loop) {
            // TODO: Add parent control
            // For Debug
            loop = 0;
            printf("Parent exit loop. PID: %d\n", getpid());
        }

        // Close all pipe at the end
        for(j = 0; j < N_CHILD; j++) {
            close(toChild[j][1]);
            close(toParent[j][0]);
        }
    }

    /* prevent zombie */
    for(i = 0; i < N_CHILD; i++)
        waitpid(shut_down[i], NULL, 0);
}


int main(int argc, char *argv[]) {
    int nChild = atoi(argv[1]);

    if(nChild < 2 || nChild > 8) {
        printf("Player range should be 2 to 8...\n");
        return 0;
    }
     // should be done in parent
    startGame(nChild);
    return 0;
}