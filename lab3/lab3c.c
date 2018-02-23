/* lab 3 C */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

pid_t getpid(void);
pid_t getppid(void);

float computeGP(char grade[])
{
  float gp;

  switch (grade[0]) {
     case 'A': gp = 4.0; break;
     case 'B': gp = 3.0; break;
     case 'C': gp = 2.0; break;
     case 'D': gp = 1.0; break;
     case 'F': gp = 0.0; break;
     default: printf("Wrong grade %s\n",grade);
	      return -1.0; /* use a negative number to indicate an error */
  }
  if (grade[1] == '+') gp = gp + 0.5;
  return gp;
}

int main(int argc, char *argv[])
{
  float gp, sum_gp = 0.0;
  int	pid, cid;
  int	num_subj, sub_code, count;
  int	i;

  num_subj = (argc-1)/2;
  printf("There are %d subjects\n",num_subj);

  pid = fork();
  if (pid < 0) { /* error occurred */
     printf("Fork Failed\n");
     exit(1);
  } else if (pid == 0) { /* child process */
     /* handle level 3 and level 4 subjects */
     count = 0;
     for (i = 1; i <= num_subj; i++) {
         sub_code = atoi(argv[i*2-1]); /* convert into integer */
	 if (sub_code >= 3000) {
            gp = computeGP(argv[i*2]);
	    if (gp >= 0) { count++; sum_gp += gp; }
	 }
     }
     printf("Child: ");
     printf("Your GPA for %d level 3/4 subjects is%5.2f\n",count,sum_gp/count);
     exit(0);
  } else { /* parent process */
     /* handle level 1 and level 2 subjects */
     count = 0;
     for (i = 1; i <= num_subj; i++) {
         sub_code = atoi(argv[i*2-1]); /* convert into integer */
	 if (sub_code < 3000) {
            gp = computeGP(argv[i*2]);
	    if (gp >= 0) { count++; sum_gp += gp; }
	 }
     }
     printf("Parent: ");
     printf("Your GPA for %d level 1/2 subjects is%5.2f\n",count,sum_gp/count);
     cid = wait(NULL);
     printf("Parent: Child %d collected\n",cid);
     exit(0);
  }
}
