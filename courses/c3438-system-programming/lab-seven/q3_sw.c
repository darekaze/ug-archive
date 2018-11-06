#include <stdio.h>

/*  1. enum  -- enumerate
    enum state {S1, S2, S3};  then S1=0; S2=1; S3=2;
    enum state vstate;        then vstate can only be S1/S2/S3;
    2. getchar(): similar to fgetc() but use stdin as its input file.
*/

void main() {
	enum state {S1, S2, S3};
	enum state current_state = S1;

	int c = getchar();

	while (c != '\n') {
		switch(current_state) {	
		case S1: 
      if(c=='a')
        current_state = S2;
      else if(c=='b')
        current_state = S1;
      break;
    case S2:
      if(c=='a')
        current_state = S3;
      else if (c=='b')
        current_state = S1;
      break;
    case S3:
      break;
		}
		c = getchar();
	}

	if( current_state == S3)
		printf("String Accpted\n");
	else
		printf("String rejected\n");
}
