/* lab 6 B receiver */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

int main()
{
    char pipename[] = "./.tmp/pp1"; /* pathname of the named pipe */
    char buf[80];
    int n, fd;

    /* open pipe for reading */
    if ((fd = open(pipename, O_RDONLY)) < 0)
    {
        printf("Pipe open error\n");
        exit(1);
    }

    while ((n = read(fd, buf, 80)) > 0)
    { /* read from pipe */
        buf[n] = 0;
        printf("<receiver> message [%s] of size %d bytes received\n", buf, n);
    }
    close(fd); /* close the pipe */
    printf("<receiver> I have completed!\n");

    exit(0);
}
