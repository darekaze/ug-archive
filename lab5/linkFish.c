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

struct Card {
    char code[3];
    int rank;
    int suit;
    struct Card *next;
};

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

void insertToHand(struct Card **head, char buf[]) {
    struct Card *curr, *newCard = (struct Card *)malloc(sizeof(struct Card));

    strcpy(newCard->code, buf);
    switch (buf[0]) {
        case 'd': case 'D': newCard->suit = 0; break;
        case 'c': case 'C': newCard->suit = 1; break;
        case 'h': case 'H': newCard->suit = 2; break;
        case 's': case 'S': newCard->suit = 3; break;
        default:            break;
    }
    switch (buf[1]) {
        case '0':           exit(EXIT_SUCCESS);
        case '2':           newCard->rank = 0; break;
        case '3':           newCard->rank = 1; break;
        case '4':           newCard->rank = 2; break;
        case '5':           newCard->rank = 3; break;
        case '6':           newCard->rank = 4; break;
        case '7':           newCard->rank = 5; break;
        case '8':           newCard->rank = 6; break;
        case '9':           newCard->rank = 7; break;
        case 't': case 'T': newCard->rank = 8; break;
        case 'j': case 'J': newCard->rank = 9; break;
        case 'q': case 'Q': newCard->rank = 10; break;
        case 'k': case 'K': newCard->rank = 11; break;
        case 'a': case 'A': newCard->rank = 12; break;
        default:            break;
    }
    newCard->next = NULL;
    if(*head == NULL || (*head)->rank < newCard->rank ||
      ((*head)->rank == newCard->rank && (*head)->suit < newCard->rank)) {
        newCard->next = *head;
        *head = newCard;
    }
    else {
        // Still have some issue (suit not sorted correctly)
        curr = *head;
        while(curr->next != NULL && curr->next->rank > newCard->rank) {
            curr = curr->next;
        }
        while(curr->next != NULL &&
          (curr->next->rank == newCard->rank && curr->next->suit > newCard->suit)) {
            curr = curr->next;
        }
        newCard->next = curr->next;
        curr->next = newCard;
    }

    
}

void initHand(int *child, struct Card **head, int num) {
    int i;
    char buf[SMALL_BUF];

    for(i = 0; i < num; i++) {
        read(child[0], buf, SMALL_BUF);
        insertToHand(head, buf);
    }
}

void rdcHand(struct Card **hand, struct Card **reduced) {
    struct Card *curr, *prev, *pair, *temp;

    prev = *hand;
    curr = prev->next;
    while(curr != NULL) {
        if(curr->next != NULL && curr->rank == curr->next->rank) {
            pair = curr;
            curr = curr->next->next;
            pair->next->next = NULL;
            if(*reduced == NULL) {
                *reduced = pair;
            } else {
                temp = *reduced;
                while(temp->next != NULL)
                    temp = temp->next;
                temp->next = pair;
            }
            prev->next = curr;
        } else {
            prev = curr;
            curr = curr->next;
        }
    }
}

void printHand(int id, struct Card *hand, char *str) {
    // sprintf for hands
    struct Card *temp;
    char cards[BIG_BUF] = "";

    temp = hand;
    while(temp != NULL) {
        (temp == hand) ? sprintf(cards,"%s", temp->code) :
        sprintf(cards, "%s %s", cards, temp->code);
        temp = temp->next;
    }
    printf("Child %d, pid %d: %s <%s> \n", id, getpid(), str, cards);
}

void makeRequest(char (*cmd), int id, struct Card *head, int pnt) {
    int tgt, i, j, num = 0;
    char wanted;
    struct Card *temp;
    int tt = nextrand();
    int ww = nextrand();

    printHand(id+1, head, "my hand");
    memmove(cmd, cmd+1, strlen(cmd));
    cmd[id] = 'x'; // exclude itself

    for(i = 0; i < strlen(cmd); i++)
        if(cmd[i] == 'p') num++;
    i = id, j = tt % num, num = 0;
    while(num < j){
        if(cmd[i] == 'p') {
            tgt = i;
            num++;
        }
        if(++i >= strlen(cmd)) i = 0;
    }

    temp = head, num = 0;
    while(temp != NULL) {
        temp = temp->next;
        num++;
    }
    num = (ww - 1) % num, temp = head;
    for(i = 0; i < num; i++) {
        temp = temp->next;
    }
    wanted = temp->code[1];

    sprintf(cmd, "%c%d%c", 'h', tgt, wanted);
    printf("Child %d, pid %d: random number %d %d, asking child %d for rank %c\n",
             id+1, getpid(), tt, ww, tgt+1, wanted);
    write(pnt, cmd, BIG_BUF);
}

void handleRequest(int id, char c, struct Card **head, int pnt) {
    struct Card *temp;
    char res[SMALL_BUF] = "";

    temp = *head;
    if(temp->code[1] == c) {
        strcpy(res, temp->code);
        *head = temp->next;
    }
    else {
        while(temp->next != NULL) {
            if(temp->next->code[1] == c) {
                strcpy(res, temp->next->code);
                temp->next = temp->next->next;
                break;
            }
            temp = temp->next;
        }
    }
    // TODO: Check if deck is empty
    if(strcmp(res, "") != 0) {
        printHand(id, *head, "new hand");
        sprintf(res, "%c%s", 'y', res);
    }
    else {
        printf("Child %d, pid %d: go fish\n", id+1, getpid());
        strcpy(res, "n");
    }
    write(pnt, res, BIG_BUF);
}


void startGame(const int N_CHILD) {
    int toParent[N_CHILD][2];
    int toChild[N_CHILD][2];
    pid_t shut_down[N_CHILD];
    char cmdBuf[BIG_BUF] = "";
    int pid, i, j, loop = 1;
    int nCard = (N_CHILD < 5) ? 7 : 5;
    
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
            struct Card *hand = NULL;
            struct Card *reduced = NULL;
            

            // usable pipe-> read: toChild[i][0] write: toParent[i][1]
            for(j = 0; j < N_CHILD; j++){
                close(toChild[j][1]);
                close(toParent[j][0]);
                if(j != i) {
                    close(toChild[j][0]);
                    close(toParent[j][1]);
                }
            }

            initrand(i+1);
            initHand(toChild[i], &hand, nCard); // Get initial card
            printHand(i+1, hand, "initial hand");
            rdcHand(&hand, &reduced);
            printHand(i+1, hand, "reduced hand");

            // while(hand != NULL)
            while(loop) {
                read(toChild[i][0], cmdBuf, BIG_BUF);
                printf("%d..%s\n", i+1, cmdBuf);
                switch(cmdBuf[0]) {
                    case 'm':
                        makeRequest(cmdBuf, i, hand, toParent[i][1]);
                        break;
                    case 'h':
                        printf("This is child %d\n", i+1);
                        handleRequest(i+1, cmdBuf[2], &hand, toParent[i][1]);
                        loop = 0;
                        break;
                    default:
                        printf("Unknown command\n");
                        loop = 0;
                        break;
                }
                // Main functions -- need while loop here

                // TODO: checkReduce 
                // 1. got card from another player then reduce
                // 2. gofish reduce
                
                // TODO: check hand is empty, which stops the child process
                // First one with most pair wins
                
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
        int k = 0, num = N_CHILD, turn = 0, tgt;
        char deck[52][3];
        int fPlayer[N_CHILD];
        int tPlayer[N_CHILD];
        char avab[N_CHILD+1];

        // usable pipe-> read: toParent[i][0] write: toChild[i][1]
        for(i = 0; i < N_CHILD; i++) {
            close(toChild[i][0]);
            close(toParent[i][1]);
        }
        
        printf("Parent: the child players are ");
        for(i = 0; i < N_CHILD; i++)
            printf("%d ", shut_down[i]);
        printf("\n");
        readDeck(deck);

        // Available player string
        avab[0] = 'm';
        for(i = 1; i <= N_CHILD; i++) {
            avab[i] = 'p';
        }
        // Initialize player hand
        for(i = 0; i < nCard; i++)
            for(j = 0; j < N_CHILD; j++)
                write(toChild[j][1], deck[k++], SMALL_BUF);

        // Player cycle
        i = 0;
        // while(loop) {

            // TODO: Add parent control
            write(toChild[turn][1], avab, BIG_BUF); // need to send the player list
            read(toParent[turn][0], cmdBuf, BIG_BUF); // read the player request then pass it to the child

            printf("p---%s\n", cmdBuf);
            tgt = (int) cmdBuf[1];
            write(toChild[1][1], cmdBuf, BIG_BUF);
            read(toParent[1][0], cmdBuf, BIG_BUF);
            printf("%c", cmdBuf[1]);
            
            // if(++turn >= num) {
            //     turn = 0;
            // }
            loop = 0;
            // printf("Parent exit loop. PID: %d\n", getpid());

        // }

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
    startGame(nChild);
    return 0;
}