#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<ctype.h>

#define K_NUM 3
typedef enum { false, true } bool;

const char *keywords[K_NUM] = {
  "var","begin","end"
};

void lexAnalyze(FILE*);
void checkKeyId(char*, FILE*);
void checkNum(char*, FILE*);
bool checkOperator(char);
bool checkSeparator(char);


int main(int argc, char const *argv[]) {
  FILE *fp;

  fp = fopen(argv[1], "r");
  if (fp == NULL) {
    printf("Error: File not found...\n");
    exit(1);
  }

  lexAnalyze(fp);

  fclose(fp);
  return 0;
}


void lexAnalyze(FILE *fp) {
  char c;

  while ((c = fgetc(fp)) != EOF) {
    if (c == ' ' || c == '\n') continue;

    if (isalpha(c)) checkKeyId(&c, fp);
    else if (isdigit(c)) checkNum(&c, fp);

    if (c == ' ' || c == '\n') continue;
    checkOperator(c);
    checkSeparator(c);
  }
  return;
}


void checkKeyId(char *c, FILE *fp) {
  char buf[100];
  int i = 0;

  do {
    buf[i++] = *c;
    *c = fgetc(fp);
  } while(isalnum(*c));
  buf[i] = '\0';

  // Check if it is keyword
  for (i = 0; i < K_NUM; i++) {
    if (strcmp(keywords[i], buf) == 0) {
      printf("KEYWORD\t\t%s\n", buf);
      return;
    }
  }
  // Otherwise, print out as ID
  printf("ID\t\t%s\n", buf);
  return;
}


void checkNum(char *c, FILE *fp) {
  char buf[100];
  int i = 0;

  do {
    buf[i++] = *c;
    *c = fgetc(fp);
  } while(isdigit(*c) || *c == '.');
  if (buf[i-1] == '.') {
    buf[i-1] = '\0';
    printf("NUM\t\t%s\n", buf);
    printf("PERIOD\n");
  } else {
    buf[i] = '\0';
    printf("NUM\t\t%s\n", buf);
  }
  return;
}


bool checkOperator(char c) {
  bool f = true;
  switch(c) {
    case '=':
      printf("ASSIGN\n"); break;
    case '+':
      printf("PLUS\n"); break;
    case '-':
      printf("MINUS\n"); break;
    case '*':
      printf("MUL\n"); break;
    case '/':
      printf("DIV\n"); break;
    default:
      f = false;
  }
  return f;
}


bool checkSeparator(char c) {
  bool f = true;
  switch(c) {
    case ';':
      printf("SEMICOLON\n"); break;
    case ',':
      printf("COMMA\n"); break;
    case '.':
      printf("PERIOD\n"); break;
    case '(':
      printf("LBRACE\n"); break;
    case ')':
      printf("RBRACE\n"); break;
    default:
      f = false;
  }
  return f;
}
