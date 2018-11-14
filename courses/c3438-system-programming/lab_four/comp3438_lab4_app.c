#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main()
{
	int fd;
	int count;
	char buf[100];

	fd = open("/dev/lab4", O_RDONLY);
	if(fd == -1)
	{
		printf("Fail to open device lab4!\n");
		goto finish;
	}

	count = read(fd, buf, 100);
	printf("%d characters are read from the kernel space:\n", count);

	buf[count]='\0';
	printf("%s\n", buf);

finish:
	close(fd);
	return 0;
}
