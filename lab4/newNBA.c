#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

typedef enum {east, west} area;

struct Team {
    area a;
    char name[4];
    char fullname[20];
    int hwin;
    int hlose;
    int rwin;
    int rlose;
    double pct;
};



int main() {
    int	infd;
    int	num_subj, in_gp, sum_gp;
    char inbuf[256];
    int	err, num_char, i;
    struct Team teams[4];
}
