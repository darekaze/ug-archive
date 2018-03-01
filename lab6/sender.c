/* lab 6 B sender */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

int main()
{
    char pipename[] = "./.tmp/pp1";   /* pathname of the named pipe */
    char mapl[] = "qwertyuiopasdfghjklzxcvbnm"; /* for encoding letter */
    char mapd[] = "1357924680";                 /* for encoding digit */
    char buf[80];
    int i, n, fd;

    /* create the named pipe */
    if (mkfifo(pipename, 0600) < 0)
    {
        printf("Pipe creation error\n");
        exit(1);
    }

    /* open pipe for writing */
    if ((fd = open(pipename, O_WRONLY)) < 0)
    {
        printf("Pipe open error\n");
        exit(1);
    }

    while (1)
    {
        printf("<sender> please enter a message\n");
        n = read(STDIN_FILENO, buf, 80); /* read a line from keyboard */
        if (n <= 0)
            break; /* EOF or error */
        buf[--n] = 0;
        printf("<sender> message [%s] is of length %d\n", buf, n);
        for (i = 0; i < n; i++) /* encrypt */
            if (buf[i] >= 'a' && buf[i] <= 'z')
                buf[i] = mapl[buf[i] - 'a'];
            else if (buf[i] >= 'A' && buf[i] <= 'Z')
                buf[i] = mapl[buf[i] - 'A'] - ('a' - 'A');
            else if (buf[i] >= '0' && buf[i] <= '9')
                buf[i] = mapd[buf[i] - '0'];
        printf("<sender> sending encrypted message [%s] to receiver\n", buf);
        write(fd, buf, n); /* send the encrypted string via pipe */
    }
    close(fd); /* close the pipe */
    printf("<sender> I have completed!\n");

    unlink(pipename); /* remove the pipe */

    exit(0);
}
