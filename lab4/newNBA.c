#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <glob.h>

#define R_WIN 1
#define R_LOSE 2
#define H_WIN 3
#define H_LOSE 4

struct team{
    struct team *next;
    char abbr[4];
    char fullname[20];
    int hwin;
    int hlose;
    int rwin;
    int rlose;
    int pct;
    double netWin;
};

void insert(struct team **head, char abbr[], char full[]) {
    struct team * newTeam = NULL;
    newTeam = (struct team *)malloc(sizeof(struct team));
    if (newTeam == NULL) {
        printf("Error: Out of memory..");
    }
    strcpy(newTeam->abbr, abbr);
    strcpy(newTeam->fullname, full);
    newTeam->next = *head;

    *head = newTeam;
}

void recordStat(struct team *head, char key[], int stat) {
    while (head) {
        if (strcmp(head->abbr, key) == 0) {
            switch(stat) {
                case 1: head->rwin++; break;
                case 2: head->rlose++; break;
                case 3: head->hwin++; break;
                case 4: head->hlose++; break;
                default: break;
            }
            return;
        }
        head = head->next;
    }
}

void calcStat(struct team *head) {
    while (head) {
        // calculate pct
        int t = (head->rwin) + (head->rlose) + (head->hwin) + (head->hlose);
        if(t != 0)
            head->pct = ((head->rwin) + (head->hwin)) * 1000 / t;
        // calculate netWin
        head->netWin = (double)((head->rwin) + (head->hwin) - (head->rlose) - (head->hlose)) / 2.0;
        head = head->next;
    }
}

void sort(struct team **head, struct team *newTeam) {
    struct team *temp = NULL;

    if(*head == NULL || newTeam->netWin >= (*head)->netWin) {
        newTeam->next = *head;
        *head = newTeam;
        return;
    }

    temp = *head;
    while(temp->next && newTeam->netWin < temp->next->netWin) {
        temp = temp->next;
    }

    newTeam->next = temp->next;
    temp->next = newTeam;
}

void sortList(struct team **head) {
    struct team *new_head = NULL;
    struct team *temp = *head;
    struct team *temp1 = NULL;

    while(temp) {
        temp1 = temp;
        temp = temp->next;
        sort(&new_head, temp1);
    }
    *head = new_head;
}

void print_list(struct team *head) {
    double hnw = head->netWin, gb;
    int rank = 1;
    int w, l;

    printf("Rank\tTeam\t\tW\tL\tPCT\tGB\tHOME\tROAD\n");
    while(head) {
        w = head->hwin + head->rwin;
        l = head->hlose + head-> rlose;
        gb = hnw - head->netWin;
        printf("%-6.d %-16s %d\t%d\t%d\t%.1lf\t%d-%d\t%d-%d\n", 
            rank, head->fullname, w, l, head->pct, gb, 
            head->hwin, head->hlose,head->rwin,head->rlose);
        head = head->next;
        rank++;
    }
    printf("\n");
}

/* --------- Main ------------ */ 

int main(int argc, char *argv[]) {
    int	infd, n;
    char inbuf[513];
    int	num_char, i, j;
    struct team *east = NULL;
    struct team *west = NULL;
    glob_t globbuff;

    infd = open(argv[1],O_RDONLY);
    if (infd < 0) {
        printf("Error: cannot open input file\n");
        exit(1);
    }

    // Read team data
    while ((num_char = read(infd,inbuf,512)) > 0) {
        /* zero means EOF, print to debug */
        i = 0;
        j = 0;
        inbuf[512] = '\0';

        while(inbuf[i] == ' ') i++; // Skip space
        // Read line
        while (i < num_char){
            char abbr[4]="", fn[21]="", area[8]="";
            abbr[3] = '\0'; fn[20] = '\0'; area[7] = '\0';
            // Read abbr
            j = 0;
            while(inbuf[i] != ' ') {
                abbr[j] = inbuf[i];
                i++;
                j++;
            }
            while(inbuf[i] == ' ') i++; // Skip space
            // Read full name
            j = 0;
            while(inbuf[i] != ' ') {
                fn[j] = inbuf[i];
                i++;
                j++;
            }
            // Skip middle part
            while(inbuf[i] == ' ') i++;
            while(inbuf[i] != ' ') i++;
            while(inbuf[i] == ' ') i++;
            // Read area
            j = 0;
            while(inbuf[i] != '\n') {
                area[j] = inbuf[i];
                i++;
                j++;
            }
            // E or W
            if(strcmp(area, "Eastern") == 0)
                insert(&east,abbr,fn);
            else if (strcmp(area, "Western") == 0)
                insert(&west,abbr,fn);
            while(inbuf[i] == ' ' || inbuf[i] == '\n') i++; // Ready for next line
        }
    }
    close(infd);

    // Read score data
    strcat(argv[2],"*");
    if(glob(argv[2], 0, NULL, &globbuff)) {
        printf("Can't find any file");
        exit(1);
    }
    for(n = 0;n < globbuff.gl_pathc; n++) {
        infd = open(globbuff.gl_pathv[n],O_RDONLY);
        while ((num_char = read(infd,inbuf,512)) > 0) {
            i = 0;
            j = 0;
            inbuf[512] = '\0';
            // Skip space
            while(inbuf[i] == ' ') i++;
            // Read line
            while (i < num_char){
                char home[4]="", road[4]="";
                char hs[4]="", rs[4]="";
                int hscore, rscore;
                
                // Read road abbr
                j = 0;
                while(inbuf[i] != ' ') {
                    road[j] = inbuf[i];
                    i++;
                    j++;
                }
                while(inbuf[i] == ' ') i++; // Skip space
                // Read road score
                j = 0;
                while(inbuf[i] != ' ') {
                    rs[j] = inbuf[i];
                    i++;
                    j++;
                }
                while(inbuf[i] == ' ') i++; // Skip space
                // Read home abbr
                j = 0;
                while(inbuf[i] != ' ') {
                    home[j] = inbuf[i];
                    i++;
                    j++;
                }
                while(inbuf[i] == ' ') i++; // Skip space
                // Read home score
                j = 0;
                while(inbuf[i] != '\n') {
                    hs[j] = inbuf[i];
                    i++;
                    j++;
                }
                hscore = atoi(hs);
                rscore = atoi(rs);

                // Compare and record score
                if(rscore > hscore) {
                    // rwin++, hlose++
                    recordStat(east, road, R_WIN);
                    recordStat(west, road, R_WIN);
                    recordStat(east, home, H_LOSE);
                    recordStat(west, home, H_LOSE);
                } else {
                    // rlose++, hwin++
                    recordStat(east, road, R_LOSE);
                    recordStat(west, road, R_LOSE);
                    recordStat(east, home, H_WIN);
                    recordStat(west, home, H_WIN);
                }
                // printf("%s %d %s %d\n\n",road,rscore,home,hscore);

                while(inbuf[i] == ' ' || inbuf[i] == '\n') i++; // Ready for next line
            }
        }
        close(infd);
    }
    globfree(&globbuff);

    calcStat(east);
    calcStat(west);
    sortList(&east);
    sortList(&west);

    printf("\nWestern Conference\n");
    print_list(west);

    printf("Eastern Conference\n");
    print_list(east);
    
    return 0;
}


