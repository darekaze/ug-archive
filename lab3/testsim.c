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
int pairs;   /* 0, 1, or 2 */

/* prototypes */
void devMode(char *[]);
void simMode(int, char *[]);

void readCards(char *[]);
void analyze(void);
char findPattern(void);
char* printPattern(char);

int simulation(int,int,char);
void randomPicker(void);
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
            int nArgs = task*3;
            int startingPt = i*3 + 1;
            if( startingPt < nArgs ){
                for(int j = startingPt; j < nArgs; j = j + (atoi(argv[0])*3)) {
                    int total = atoi(argv[j]);
                    int seed = atoi(argv[j+1]);
                    int ptrn = argv[j+2][0];
                    int res = simulation(total, seed, ptrn);
                    double prop = ((double)res / (double)total) * 100;
                    printf("Child %d, pid %d: seed = %d, %d %s out of %d hands, p = %.3f\%\n",
                        i+1, getpid(), seed, res, printPattern(ptrn), total, prop );
                }
            } else
                printf("Child %d, pid %d: no task\n", i+1, getpid());
            exit(0);
        }
    }
    int stat;
    for(int i = 0; i < atoi(argv[0]); i++)
        wait(&stat);
}

/* simulate num times and return the no. of occurrance */
int simulation(int num, int seed, char r) {
    int count = 0;
    char res;
    srand(seed);
    for(int i = 0; i < num; i++) {
        randomPicker();
        analyze();
        res = findPattern();
        if(r == res)
            count++;
    }
    return count;
}

/* Pick 5 random card */
void randomPicker(void) {
    for (int i = 0; i < RANKS; i++) 
        numInRank[i] = 0;
    for (int i = 0; i < SUITS; i++)
        numInSuit[i] = 0;

    int used[RANKS*SUITS] = {0};
    for (int i = 0; i < NUM_CARDS;i++) {
        int p = rand() % 52;
        while(used[p]) p = rand() % 52;
        used[p] = 1;
        numInRank[p/4]++;
        numInSuit[p%4]++;
    }
}

/* Read the 5 cards from terminal input */
void readCards(char *card[]) {
    bool used[RANKS][SUITS];
    char ch, rank_ch, suit_ch;
    int rank, suit;
    bool bad;

    for (rank = 0; rank < RANKS; rank++) {
        numInRank[rank] = 0;
        for (suit = 0; suit < SUITS; suit++)
            used[rank][suit] = false;
    }
    for (suit = 0; suit < SUITS; suit++)
        numInSuit[suit] = 0;
    for(int i = 0;i < NUM_CARDS; i++) {
        bad = false;
        suit_ch = card[i][0];
        rank_ch = card[i][1];

        switch (suit_ch) {
            case 'd': case 'D': suit = 0; break;
            case 'c': case 'C': suit = 1; break;
            case 'h': case 'H': suit = 2; break;
            case 's': case 'S': suit = 3; break;
            default:            bad = true;
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
            default:            bad = true;
        }
        if (bad) {
            printf("Error: Bad card detected\n");
            exit(EXIT_FAILURE);
        } 
        else if (used[rank][suit]){
            printf("ERROR: Duplicate card\n");
            exit(EXIT_FAILURE);
        }
        else {
            numInRank[rank]++;
            numInSuit[suit]++;
            used[rank][suit] = true;
        }
  }
}

void analyze(void) {
    int sequence = 0;
    int rank, suit;

    straight = false;
    flush = false;
    four = false;
    three = false;
    pairs = 0;

    /* check flush */
    for (suit = 0; suit < SUITS; suit++)
        if (numInSuit[suit] == NUM_CARDS)
        flush = true;

    /* check straight */
    rank = 0;
    while (numInRank[rank] == 0) rank++;
        for (; rank < RANKS && numInRank[rank] > 0; rank++)
            sequence++;
    if (sequence == NUM_CARDS) {
        straight = true;
        return;
    }

    /* check 4,3 and pairs */
    for (rank = 0; rank < RANKS; rank++) {
        if (numInRank[rank] == 4) four = true;
        if (numInRank[rank] == 3) three = true;
        if (numInRank[rank] == 2) pairs++;
    }
}

/* Output the highest winning pattern*/
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

/* Print out the winning pattern*/
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