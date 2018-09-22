#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
int main(void)
{
    pid_t pid;
    int r;
    char buf[1024];
    char *cp;
    int readpipe[2];
    int writepipe[2];
    int a;
    int b;
    a=pipe(readpipe);
    b=pipe(writepipe);
    // check a and b
    pid=fork();
    // check pid
    if(pid==0)
    { //CHILD PROCESS
        close(readpipe[1]);
        close(writepipe[0]);
        read(readpipe[0],buf,sizeof(buf));
        printf("\nREAD = %s",buf);
        close(readpipe[0]);
        cp="YES\n";
        write(writepipe[1],cp,strlen(cp)+1);
        close(writepipe[1]);
    }
    else
    { //PARENT PROCESS
        close(readpipe[0]);
        close(writepipe[1]);
        cp="HI!! YOU THERE";
        write(readpipe[1],cp,strlen(cp)+1);
        close(readpipe[1]);
        read(writepipe[0],buf,sizeof(buf));
        printf("\nACK RECEIVED %s",buf);
        close(writepipe[0]);
    }
    return 0;
}
