#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

struct team{
    struct team *next;
    char abbr[4];
    char fullname[20];
    int hwin;
    int hlose;
    int rwin;
    int rlose;
    double pct;
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


// Use Linked list instead
int main(int argc, char *argv[]) {
    int	infd;
    char inbuf[513];
    int	num_char, i, j;
    struct team *east = NULL;
    struct team *west = NULL;

    infd = open(argv[1],O_RDONLY); /* read only */
    if (infd < 0) {
        printf("Error in opening input file\n");
        exit(1);
    }

    // Read team data
    while ((num_char = read(infd,inbuf,512)) > 0) {
        /* zero means EOF, print to debug */
        printf("Number of bytes read = %d\n",num_char);
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
            printf("abbr: %s\n", abbr);
            // Skip space
            while(inbuf[i] == ' ') i++;
            // Read full name
            j = 0;
            while(inbuf[i] != ' ') {
                fn[j] = inbuf[i];
                i++;
                j++;
            }
            printf("fn: %s\n", fn);
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
            printf("area: %s\n", area);
            // E or W
            if(strcmp(area, "Eastern") == 0)
                insert(&east,abbr,fn);
            else if (strcmp(area, "Western") == 0)
                insert(&west,abbr,fn);
            // Ready for next line
            while(inbuf[i] == ' ' || inbuf[i] == '\n') i++;
        }
        // TODO: Rest of the part
    }
    close(infd);

    print_list(east);
    print_list(west);

    

    return 0;
}


