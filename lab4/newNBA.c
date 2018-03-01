#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

typedef enum {east, west} area;

typedef struct team{
    area a;
    char name[4];
    char fullname[20];
    int hwin;
    int hlose;
    int rwin;
    int rlose;
    double pct;
    struct team* next;
} teamList;


// Use Linked list instead
int main() {
    int	infd;
    int	num_subj, in_gp, sum_gp;
    char inbuf[256];
    int	err, num_char, i;
}
