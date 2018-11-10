#include <stdio.h>

#define ORD_a 97

int main() {
	enum state {S1, S2, S3};
	enum state current_state = S1;
  int st[3][2] = {
    {S2, S1},
    {S3, S1},
    {S3, S3}
  };

	int c = getchar() - ORD_a;

	while (c != ('\n' - ORD_a)) {
    current_state = st[current_state][c];
    if (current_state == S3) break;
		c = getchar() - ORD_a;
	}

	if( current_state == S3)
		printf("String Accpted\n");
	else
		printf("String rejected\n");

	return 0;
}
