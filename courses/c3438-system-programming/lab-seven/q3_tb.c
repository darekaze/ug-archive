#include <stdio.h>

void main() {
	enum state {S1, S2, S3};
	enum state current_state = S1;
  int st[3][2] = {
    {S2, S1},
    {S3, S1},
    {S3, S3}
  };

	int c = getchar() - 97;

	while (c != ('\n' - 97)) {
    current_state = st[current_state][c];
    if (current_state == S3) break;
		c = getchar() - 97;
	}

	if( current_state == S3)
		printf("String Accpted\n");
	else
		printf("String rejected\n");
}
