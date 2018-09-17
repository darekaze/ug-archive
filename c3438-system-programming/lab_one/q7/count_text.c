#include <stdio.h>
#include <ctype.h>

int main() {
    int charCount = 0, wordCount = 0, lnCount = 0;
    int isSpc = 1, ch, last = '\n';

    while ((ch = getchar()) != '*') {
        last = ch;
        charCount++;
        if (isspace(ch)) {
            isSpc = 1;
            lnCount += (ch == '\n') ? 1 : 0;
        } else {
            wordCount += isSpc;
            isSpc = 0;
        }
    }

    if(last != '\n') lnCount++;
    printf("\n\n"
        "The number of Char is: %d\n"
        "The number of Word is: %d\n"
        "The number of Line is: %d\n"
        , charCount, wordCount, lnCount
    );

    return 0;
}
