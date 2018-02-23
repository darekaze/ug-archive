#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

pid_t getpid(void);

#define RANKS 13
#define SUITS 4
#define NUM_CARDS 5
#define TOGGLE_DEV 7

typedef enum {false, true} bool;

/* variables */
int numInRank[RANKS];
int numInSuit[SUITS];
bool straight, flush, four, three;
int pairs;   /* can be 0, 1, or 2 */

/* prototypes */
void devMode(char *[]);
void simMode(int, char *[]);

void readCards(char *[]);
void analyze(void);
char findPattern(void);
char* printPattern(char);

int simulation(int,int,char);
void randomPicker(int);
void tester(int);

int main(int argc, char *argv[]) {
    int numChild = atoi(argv[1]);
    int myid = getpid();
    printf("Parent, pid %d: %d children, ", myid, numChild);
    if(argc == TOGGLE_DEV) {
        printf("development mode\n");
        devMode(argv+1);
    } else {
        int task = (argc-2)/3;
        printf("%d tasks\n", task);
        simMode(task, argv+1);
    }
    return 0;
}


/* Development mode */
void devMode(char *argv[]) {
    int pid;
    for(int i = 0; i < atoi(argv[0]); i++) {
        pid = fork();
        if(pid < 0) {
            printf("Error");
            exit(1);
        } 
        else if (pid == 0) {
            readCards(argv+1);
            analyze();
            char t = findPattern();
            printf("Child %d, pid %d: hand is <%s %s %s %s %s>, %s\n",
                    i+1,getpid(),argv[1],argv[2],argv[3],argv[4],argv[5],
                    printPattern(t));
            exit(0);
        }
    }
    int stat;
    for(int i = 0; i < atoi(argv[0]); i++)
        wait(&stat);
}


/* Simulation mode */
void simMode(int task, char *argv[]) {

    int pid;
    for(int i = 0; i < atoi(argv[0]); i++) {
        pid = fork();
        if(pid < 0) {
            printf("Error");
            exit(1);
        } 
        else if (pid == 0) {
            for(int j = 1+(i*3); j < (task*3); j = j + (atoi(argv[0])*3)) {
                int total = atoi(argv[j]);
                int seed = atoi(argv[j+1]);
                int ptrn = argv[j+2][0];
                int res = simulation(total, seed, ptrn);
                printf("Child %d, pid %d: seed = %d, %d %s out of %d hands, p = %.3f\n",
                     i+1, getpid(), seed, res, printPattern(ptrn), total, (float)res / (float)total );
            }
            exit(0);
        }
    }
    int stat;
    for(int i = 0; i < atoi(argv[0]); i++)
        wait(&stat);
}

int simulation(int num, int seed, char r) {
    int count = 0;
    char res;
    for(int i = 0; i < num; i++) {
        // randomPicker(seed);
        tester(seed);
        analyze();
        res = findPattern();
        if(r == res)
            count++;
    }
    return count;
}

void tester(int seed) {
    for (int i = 0; i < RANKS; i++) 
        numInRank[i] = 0;
    for (int i = 0; i < SUITS; i++)
        numInSuit[i] = 0;

    // numInRank[0]++;
    // numInRank[1]++;
    numInRank[2]++;
    numInRank[3]=2;
    numInRank[4]=2;
    numInSuit[1]=2;
    numInSuit[2]=3;

}

// Big bug here!!! cannot random 
void randomPicker(int seed) {
    for (int i = 0; i < RANKS; i++) 
        numInRank[i] = 0;
    for (int i = 0; i < SUITS; i++)
        numInSuit[i] = 0;

    int used[RANKS*SUITS] = {0};
    srand((unsigned)seed);
    for (int i = 0; i < NUM_CARDS;i++) {
        int p = rand() % 52;
        while(used[p]) p = rand() % 52;
        used[p] = 1;
        numInRank[p/4]++;
        if (numInRank[p/4] >1) {
            printf("w");
        }
        numInSuit[p%4]++;
        printf("%d ",p);
    }
    printf("\n");
}

void readCards(char *card[]) {
    bool card_exists[RANKS][SUITS];
    char ch, rank_ch, suit_ch;
    int rank, suit;
    bool bad_card;

    for (rank = 0; rank < RANKS; rank++) {
        numInRank[rank] = 0;
        for (suit = 0; suit < SUITS; suit++)
            card_exists[rank][suit] = false;
    }

    for (suit = 0; suit < SUITS; suit++)
        numInSuit[suit] = 0;

    for(int i = 0;i < NUM_CARDS; i++) {
        bad_card = false;
        suit_ch = card[i][0];
        rank_ch = card[i][1];

        switch (suit_ch) {
            case 'd': case 'D': suit = 0; break;
            case 'c': case 'C': suit = 1; break;
            case 'h': case 'H': suit = 2; break;
            case 's': case 'S': suit = 3; break;
            default:            bad_card = true;
        }
        switch (rank_ch) {
            case '0':           exit(EXIT_SUCCESS);
            case '2':           rank = 0; break;
            case '3':           rank = 1; break;
            case '4':           rank = 2; break;
            case '5':           rank = 3; break;
            case '6':           rank = 4; break;
            case '7':           rank = 5; break;
            case '8':           rank = 6; break;
            case '9':           rank = 7; break;
            case 't': case 'T': rank = 8; break;
            case 'j': case 'J': rank = 9; break;
            case 'q': case 'Q': rank = 10; break;
            case 'k': case 'K': rank = 11; break;
            case 'a': case 'A': rank = 12; break;
            default:            bad_card = true;
        }
        if (bad_card) {
            printf("Error: Bad card detected\n");
            exit(EXIT_FAILURE);
        } 
        else if (card_exists[rank][suit]){
            printf("ERROR: Duplicate card\n");
            exit(EXIT_FAILURE);
        }
        else {
            numInRank[rank]++;
            numInSuit[suit]++;
            card_exists[rank][suit] = true;
        }
  }
}

void analyze(void) {
    int num_consec = 0;
    int rank, suit;

    straight = false;
    flush = false;
    four = false;
    three = false;
    pairs = 0;

    /* check for flush */
    for (suit = 0; suit < SUITS; suit++)
        if (numInSuit[suit] == NUM_CARDS)
        flush = true;

    /* check for straight */
    rank = 0;
    while (numInRank[rank] == 0) rank++;
        for (; rank < RANKS && numInRank[rank] > 0; rank++)
            num_consec++;
    if (num_consec == NUM_CARDS) {
        straight = true;
        return;
    }

    /* check for 4-of-a-kind, 3-of-a-kind, and pairs */
    for (rank = 0; rank < RANKS; rank++) {
        if (numInRank[rank] == 4) four = true;
        if (numInRank[rank] == 3) three = true;
        if (numInRank[rank] == 2) pairs++;
    }
}

char findPattern(void) {
    char res;
    if (straight && flush) res = 'R';
    else if (four)         res = '4';
    else if (three &&
            pairs == 1)    res = 'H';
    else if (flush)        res = 'F';
    else if (straight)     res = 'S';
    else if (three)        res = '3';
    else if (pairs == 2)   res = 'T';
    else if (pairs == 1)   res = '2';
    else                   res = 'X';

    return res;
}

char* printPattern(char r) {
    if (r == 'R')       return "Royal-flush";
    else if (r == '4')  return "Four-of-a-kind";
    else if (r == 'H')  return "Full-house";
    else if (r == 'F')  return "Flush";
    else if (r == 'S')  return "Straight";
    else if (r == '3')  return "Three-of-a-kind";
    else if (r == 'T')  return "Two-pairs";
    else if (r == '2')  return "Pair";
    else                return "High-card";

    printf("\n");
}