/* lab 4 A-2 */
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

int main()
{
  int	infd;
  int	num_subj, in_gp, sum_gp;
  char	inbuf[80];
  int	err, num_char, i;

  infd = open("grade.txt",O_RDONLY); /* read only */
  if (infd < 0) {
      printf("Error in opening input file\n");
      exit(1);
  }

  sum_gp = 0; num_subj = 0;
  while ((num_char = read(infd,inbuf,80)) > 0) {
      /* zero means EOF, print to debug */
      printf("Number of bytes read = %d\n",num_char);
      i = 0;
      /* skip over other characters */
      while (i < num_char && (inbuf[i] < 'A' || inbuf[i] > 'Z')) i++;
      while (i < num_char) {
         err = 0;
         switch (inbuf[i]) {
            case 'A': in_gp = 4; break;
            case 'B': in_gp = 3; break;
            case 'C': in_gp = 2; break;
            case 'D': in_gp = 1; break;
            case 'F': in_gp = 0; break;
            default: fprintf(stderr, "Sorry, wrong grade %c ignored\n", inbuf[i]);
		  err = 1; break;
	    /* this error message will go to the screen instead of the file */
         }
         if (err == 0) {
            num_subj++;
            sum_gp = sum_gp + in_gp;
            printf("Grade for subject %d: %c\n", num_subj, inbuf[i]);
	 }
	 i++;
	 while (i < num_char && (inbuf[i] < 'A' || inbuf[i] > 'Z')) i++;
      }
  }
  printf("Your GPA for %d subjects is %5.2f\n", num_subj, (float)sum_gp/(float)num_subj);
  close(infd);
}

