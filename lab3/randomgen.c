#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define RANKS 13
#define SUITS 4
#define NUM_CARDS 5

int numInRank[RANKS];
int numInSuit[SUITS];

int main() {
    unsigned seed;
    int r,a,b;
    int used[RANKS*SUITS] = {0};
    printf("Input a random number seed: ");
    scanf("%u",&seed);
    srand(seed);
    for(a=0;a<20;a++){
        for(b=0;b<5;b++){
            r=rand() % 52;
            printf("%d\t",r);
        }
        putchar('\n');
    }
    return(0);
}
