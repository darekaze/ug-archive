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

/* prototype */
void initrand(int);
int nextrand();
void readDeck(char(*)[3]);
void insertToHand(struct Card**, char []);
void initHand(int*, struct Card**, int);
int rdcHand(struct Card**, struct Card**);
void printHand(int, struct Card*, char*);
void makeRequest(int, char(*), struct Card*, int);
void handleRequest(int, char c, struct Card **, struct Card *, int pnt);
void handleResult(int, char(*), struct Card**, struct Card**, int);
void startGame(const int N_CHILD);

/* Main */
int main(int argc, char *argv[]) {
    int nChild = atoi(argv[1]);

    if(nChild < 2 || nChild > 8) {
        printf("Player range should be 2 to 8...\n");
        return 0;
    }
    startGame(nChild);
    return 0;
}

/* Functions */
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

int rdcHand(struct Card **hand, struct Card **reduced) {  // Bug here!!!!!
    struct Card *curr, *prev, *pair, *temp;
    int isChange = 0;

    prev = *hand;
    curr = prev->next;
    while(curr != NULL) {
        if(prev->rank == curr->rank) {
            pair = prev;
            prev = curr->next;
            *hand = prev;
            curr = (prev != NULL) ? prev->next : NULL;
            pair->next->next = NULL;
            if(*reduced == NULL) {
                *reduced = pair;
            } else {
                temp = *reduced;
                while(temp->next != NULL)
                printf("%s->", temp->code);
                    temp = temp->next;
                temp->next = pair;
            }
            isChange = 1;
        }
        else if(curr->next != NULL && curr->rank == curr->next->rank) {
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
            isChange = 1;
        } else {
            prev = curr;
            curr = curr->next;
        }
    }
    return isChange;
}

void printHand(int id, struct Card *hand, char *str) {
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

void makeRequest(int id, char (*cmd), struct Card *head, int pnt) {
    int tgt, i, j, num = 0;
    char wanted;
    struct Card *temp;
    int tt = nextrand();
    int ww = nextrand();

    printHand(id+1, head, "my hand");
    memmove(cmd, cmd+1, strlen(cmd));
    cmd[id] = 'x'; // exclude itself
    // printf("%d+++%s\n", id+1,cmd);

    i = 0, num = 0;
    while(num < tt){
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

void handleRequest(int id, char c, struct Card **head, struct Card *reduced, int pnt) {
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
    
    if(strcmp(res, "") != 0) {
        char *tmp = strdup(res), *tpp;
        int count = 0;

        printHand(id+1, *head, "new hand");
        strcpy(res,"y");
        if(*head == NULL) {
            strcat(res, "n");
            temp = reduced;
            while(temp != NULL) {
                temp = temp->next;
                count++;
            }
            sprintf(tpp, "won %d pairs", count);
            printHand(id+1, reduced, tpp);
        } else strcat(res, "y");
        strcat(res, tmp);
        free(tmp);
    }
    else {
        printf("Child %d, pid %d: go fish\n", id+1, getpid());
        strcpy(res, "ny");
    }
    write(pnt, res, BIG_BUF);
}

void handleResult(int id, char (*cmd), struct Card **hand, struct Card **reduced, int pnt) {
    memmove(cmd, cmd+2, strlen(cmd));
    if(strcmp(cmd, "") != 0){
        insertToHand(hand, cmd);
        printHand(id+1, *hand, "new hand");
        if(rdcHand(hand, reduced))
            printHand(id+1, *hand, "reduced hand");
    }
    if(*hand == NULL){
        struct Card *temp;
        int count = 0;
        char *tpp;
        strcpy(cmd,"e");
        temp = *reduced;
            while(temp != NULL) {
                printf("%s->",temp->code);
                temp = temp->next;
                count++;
            }
        sprintf(tpp, "won %d pairs", count);
    }
    write(pnt, cmd, BIG_BUF);
    // TODO: check hand is empty, which stops the child process
    // First one with most pair wins
}


void startGame(const int N_CHILD) {
    int toParent[N_CHILD][2], toChild[N_CHILD][2];
    pid_t shut_down[N_CHILD];
    int pid, i, j, nCard = (N_CHILD < 5) ? 7 : 5;
    
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
            int loop = 1;

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
            if(rdcHand(&hand, &reduced))
                printHand(i+1, hand, "reduced hand");

            // while(hand != NULL)
            while(loop) {
                char cmdBuf[BIG_BUF] = "";
                read(toChild[i][0], cmdBuf, BIG_BUF);
                // printf("%d..%s\n", i+1, cmdBuf);
                switch(cmdBuf[0]) {
                    case 'm':
                        makeRequest(i, cmdBuf, hand, toParent[i][1]);
                        break;
                    case 'h':
                        handleRequest(i, cmdBuf[2], &hand, reduced, toParent[i][1]);
                        if(hand == NULL) {
                            printf("FUCKKKKKKK\n");
                            loop = 0;
                        }
                        break;
                    case 'y': case 'n':
                        handleResult(i, cmdBuf, &hand, &reduced, toParent[i][1]);
                        if(hand == NULL) {
                            printf("FUCKKKKKKK\n");
                            loop = 0;
                        }
                        break;
                    default:
                        printf("Unknown command\n");
                        loop = 0;
                        break;
                }
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
        int k = 0, turn = 0, loop = 18;
        int num = N_CHILD, tgt;
        char deck[52][3];
        int fPlayer[N_CHILD], tPlayer[N_CHILD];
        char avab[N_CHILD+1], cmdBuf[BIG_BUF] = "";

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

        // Play cycle
        i = 0;
        while(loop) {
            // start turn
            write(toChild[turn][1], avab, BIG_BUF);
            read(toParent[turn][0], cmdBuf, BIG_BUF);

            // handle request
            tgt = (int)cmdBuf[1] - '0';
            write(toChild[tgt][1], cmdBuf, BIG_BUF);
            read(toParent[tgt][0], cmdBuf, BIG_BUF);

            // handle result (include player status and result)
            if(cmdBuf[0] == 'n' && k <= 52) {
                sprintf(cmdBuf,"%s%s", cmdBuf, deck[k++]);
            } else if(cmdBuf[1] == 'n') {
                // TODO: read the winning result (reduced card deck)
            }
            // printf("p--%s\n", cmdBuf);
            write(toChild[turn][1], cmdBuf, BIG_BUF);
            read(toParent[turn][0], cmdBuf, BIG_BUF);

            // win?

            // printf("%c", cmdBuf[1]);
            
            if(++turn >= num) {
                turn = 0;
            }
            loop = loop - 1;
            // printf("Parent exit loop. PID: %d\n", getpid());

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
