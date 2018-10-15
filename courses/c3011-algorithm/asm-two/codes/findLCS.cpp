#include <iostream>
#include <string>

using namespace std;

#define ULI     0
#define UP      1
#define LEFT    2
#define IN      3

string findLCS(string X, string Y, string Z) {
    const int lx = X.length();
    const int ly = Y.length();
    const int lz = Z.length();

    // set Score table and traceback table (require g++ or c++11 above)
    int score[lx+1][ly+1][lz+1];
    uint8_t trace[lx+1][ly+1][lz+1];
    int a, b, c;

    // Initialize the base cases
    for(a = 1; a < lx+1; a++) {
        for(b = 0; b < ly+1; b++) {
            score[a][b][0] = 0;
            trace[a][b][0] = UP;
        }
    }
    for(b = 1; b < ly+1; b++) {
        for(c = 0; c < lz+1; c++) {
            score[0][b][c] = 0;
            trace[0][b][c] = LEFT;
        }
    }
    for(c = 1; c < lz+1; c++) {
        for(a = 0; a < lx+1; a++) {
            score[a][0][c] = 0;
            trace[a][0][c] = IN;
        }
    }

    // Fill in the score and traceback table based on the recurrence for an LCS
    for(a = 1; a < lx+1; a++)
    for(b = 1; b < ly+1; b++)
    for(c = 1; c < lz+1; c++) {
        if(X[a-1] == Y[b-1] && X[a-1] == Z[c-1]) {
            score[a][b][c] = score[a-1][b-1][c-1] + 1;
            trace[a][b][c] = ULI;
        }
        else {
            // Find max
            score[a][b][c] = score[a-1][b][c];
            trace[a][b][c] = UP;
            if(score[a][b-1][c] >= score[a][b][c]) {
                score[a][b][c] = score[a][b-1][c];
                trace[a][b][c] = LEFT;
            }
            if (score[a][b][c-1] >= score[a][b][c]) {
                score[a][b][c] = score[a][b][c-1];
                trace[a][b][c] = IN;
            }
        }
    }

    // Construct the LCS using the traceback table
    string lcs = "";
    for(a=lx, b=ly, c=lz; a>0 || b>0 || c>0; ) {
        switch(trace[a][b][c]) {
            case ULI:
                a--; b--; c--;
                lcs.insert(0, 1, X[a]);
                break;
            case UP:
                a--; break;
            case LEFT:
                b--; break;
            case IN:
                c--; break;
        }
    }

    return lcs;
}

int main(int argc, char const *argv[]) {
    try {
        string lcs = findLCS(argv[1], argv[2], argv[3]);
        cout << lcs << endl;
    } catch (const char* msg) {
        cerr << msg << endl;
    }

    return 0;
}
