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
void cardGenerator(void);
void readCards(char *[]);
void analyze(void);
void printPattern(void);
char findPattern(void);
void record(char); // record the desired output
void simulation(int,int,char); // simulation
void devMode(char *[]);
void simMode(char *[]);


int main(int argc, char *argv[]) {
    int numChild = atoi(argv[1]);
    int myid = getpid();
    printf("Parent, pid %d: %d children, ", myid, numChild);
    if(argc == TOGGLE_DEV) {
        printf("development mode\n");
        devMode(argv+1);
    } else {
        printf("%d tasks\n", (argc-2)/3);
        simMode(argv+1);
    }
    // include child process
    // for (;;) {
    //     readCards();
    //     analyze();
    //     printPattern();
    // }
}

void devMode(char *argv[]) {
    int pid;
    for(int i = 0; i < atoi(argv[0]); i++) {
        pid = fork();
        if(pid < 0) {
            printf("Error");
            exit(1);
        } else if (pid == 0) {
            readCards(argv+1);
            analyze();
            printf("Child %d, pid %d: hand is <%s %s %s %s %s>, ",
                    i+1,getpid(),argv[1],argv[2],argv[3],argv[4],argv[5]);
            printPattern();
            exit(0);
        }
    }
    int stat;
    for(int i = 0; i < atoi(argv[0]); i++) {
        wait(&stat);
    }
}

void simMode(char *argv[]) {

}


void readCards(char *card[]){
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
        switch (suit_ch) {
            case 'c': case 'C': suit = 0; break;
            case 'd': case 'D': suit = 1; break;
            case 'h': case 'H': suit = 2; break;
            case 's': case 'S': suit = 3; break;
            default:            bad_card = true;
        }

        rank_ch = card[i][1];
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

/**********************************************************
 * analyze: Determines whether the hand contains a   *
 *               straight, a flush, four-of-a-kind,       *
 *               and/or three-of-a-kind; determines the   *
 *               number of pairs; stores the results into *
 *               the external variables straight, flush,  *
 *               four, three, and pairs.                  *
 **********************************************************/
void analyze(void)
{
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


void printPattern(void)
{
  if (straight && flush) printf("Straight flush");
  else if (four)         printf("Four of a kind");
  else if (three &&
           pairs == 1)   printf("Full house");
  else if (flush)        printf("Flush");
  else if (straight)     printf("Straight");
  else if (three)        printf("Three of a kind");
  else if (pairs == 2)   printf("Two pairs");
  else if (pairs == 1)   printf("Pair");
  else                   printf("High card");

  printf("\n");
}