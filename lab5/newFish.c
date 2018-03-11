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


int main(int argc, char *argv[]) {
    char inbuf[BUFFERSIZE];
    const int NUM_CHILD = atoi(argv[1]);
    int child_to_parent[2];
    int parent_to_child[2];
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

    return 0;
}