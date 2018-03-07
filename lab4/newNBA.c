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
    double pct;
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

void print_list(struct team *head) {
    printf("H->");
    while(head) {
        printf("%s->", head->abbr);
        head = head->next;
    }
    printf("|||\n");
}

void recordStat(struct team *head, char key[], int stat) {
    while (head != NULL) {
        if (strcmp(head->abbr, key) == 0) {
            // printf("key:%s has found\n", key);
            switch(stat) {
                case 1: head->rwin++; break;
                case 2: head->rlose++; break;
                case 3: head->hwin++; break;
                case 4: head->hlose++; break;
                default: break;
            }
            // printf("%s Stat: %d-%d, %d-%d\n",key,head->rwin,head->rlose,head->hwin,head->hlose);
            return;
        }
        head = head->next;
    }
}

// Use Linked list instead
int main(int argc, char *argv[]) {
    int	infd, n;
    char inbuf[513];
    int	num_char, i, j;
    struct team *east = NULL;
    struct team *west = NULL;
    glob_t globbuff;

    infd = open(argv[1],O_RDONLY); /* read only */
    if (infd < 0) {
        printf("Error in opening input file\n");
        exit(1);
    }

    // Read team data
    while ((num_char = read(infd,inbuf,512)) > 0) {
        /* zero means EOF, print to debug */
        i = 0;
        j = 0;
        inbuf[512] = '\0';

        // Skip space
        while(inbuf[i] == ' ') i++;
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
            // Skip space
            while(inbuf[i] == ' ') i++;
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
            // Ready for next line
            while(inbuf[i] == ' ' || inbuf[i] == '\n') i++;
        }
    }
    close(infd);

    // print_list(east);
    // print_list(west);

    // Read score data
    strcat(argv[2],"*");
    if(glob(argv[2], 0, NULL, &globbuff))
        printf("Can't find any file");
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

    // TODO: Calculation part

    
    // Sort part
    // Print Part
    
    
    return 0;
}


