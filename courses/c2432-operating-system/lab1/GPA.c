#include <stdio.h>

int main(int argc, char *argv[]) {
    /* argv[0] is the name of the program */
    printf("This program name is %s\n", argv[0]);
    printf("There are %d inputs\n\n", argc-1);
    polyU(argc-1,argv+1);
    systemA(argc-1,argv+1);
    systemB(argc-1,argv+1);
    return 0;
}

/*PolyU system*/
void polyU(int num_subj, char *argv[]){
    int validNum = 0;
    float in_gp, sum_gp = 0.0;
    
    printf("PolyU:\n");
    for (int i = 0; i < num_subj; i++) {
        char in_grade = argv[i][0]; /* get first character */
        int boo = 1; /* boolean of correct input */
        switch (in_grade) {
            case 'A': in_gp = 4.0; break;
            case 'B': in_gp = 3.0; break;
            case 'C': in_gp = 2.0; break;
            case 'D': in_gp = 1.0; break;
            case 'F': in_gp = 0.0; break;
            default: 
                printf("Wrong grade %s\n", argv[i]);
                boo--;
        }
        if(boo == 0) continue;
        printf("Grade for subject %d is %c%c, ", i+1, in_grade, argv[i][1]);
        if (argv[i][1] == '-' || (in_grade == 'F' && argv[i][1] == '+')) {
            printf("invalid\n");
        } 
        else{
            if (argv[i][1] == '+')
                in_gp += 0.5;
            sum_gp += in_gp;
            validNum++;
            printf("GP%5.2f\n",in_gp);
        }
    }

    float res = sum_gp/validNum;
    if(res>4.0) res = 4.0;

    printf("Your GPA for %d subjects is%5.2f\n\n", validNum, res);
}

/*Grading system A*/
void systemA(int num_subj, char *argv[]){
    int validNum = 0, in_gp;
    float sum_gp = 0.0;
    
    printf("System A:\n");
    for (int i = 0; i < num_subj; i++) {
        char in_grade = argv[i][0]; /* get first character */
        int boo = 1; /* boolean of correct input */
        switch (in_grade) {
            case 'A': in_gp = 11; break;
            case 'B': in_gp = 8; break;
            case 'C': in_gp = 5; break;
            case 'D': in_gp = 2; break;
            case 'F': in_gp = 0; break;
            default: 
                printf("Wrong grade %s\n", argv[i]);
                boo--;
        }
        if(boo == 0) continue;
        printf("Grade for subject %d is %c%c, ", i+1, in_grade, argv[i][1]);
        if (in_grade != 'D' && in_grade != 'F') {
            if (argv[i][1] == '+') in_gp++;
            else if(argv[i][1] == '-') in_gp--;
        } else if(argv[i][1] == '+' || argv[i][1] == '-'){
            printf("invalid\n");
            continue;
        }
        sum_gp += in_gp;
        validNum++;
        printf("GP %d\n",in_gp);
    }

    printf("Your GPA for %d subjects is %5.2f\n\n", validNum, sum_gp*1.0/validNum);
}

/*Grading system B*/
void systemB(int num_subj, char *argv[]){
    int validNum = 0;
    float in_gp, sum_gp = 0.0;
    
    printf("System B:\n");
    for (int i = 0; i < num_subj; i++) {
        char in_grade = argv[i][0]; /* get first character */
        int boo = 1; /* boolean of correct input */
        switch (in_grade) {
            case 'A': in_gp = 4.0; break;
            case 'B': in_gp = 3.0; break;
            case 'C': in_gp = 2.0; break;
            case 'D': in_gp = 1.0; break;
            case 'F': in_gp = 0.0; break;
            default: 
                printf("Wrong grade %s\n", argv[i]);
                boo--;
        }
        if(boo == 0) continue;
        printf("Grade for subject %d is %c%c, ", i+1, in_grade, argv[i][1]);
        if ((in_grade == 'A' || in_grade == 'F') && argv[i][1] == '+') {
            printf("invalid\n");
        } 
        else{
            if (argv[i][1] == '+')
                in_gp += 0.3;
            else if(argv[i][1] == '-')
                in_gp -= 0.3;
            sum_gp += in_gp;
            validNum++;
            printf("GP%5.2f\n",in_gp);
        }
    }
    printf("Your GPA for %d subjects is%5.2f\n\n", validNum, sum_gp/validNum);
}