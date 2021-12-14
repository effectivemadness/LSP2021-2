#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <strings.h>
#include <string.h>

int main()
{
	int fd, n;
	char buf[10];
	if ((fd = open("myfifo", O_RDWR)) < 0)
	{
		perror("open");
		exit(1);
	}
	bzero(buf, sizeof(buf));
	strcpy(buf, "Hello");
	printf("FIFO2 : Sent a message: %s\n", buf);
	if (write(fd, buf, strlen(buf)) < 0)
	{
		perror("write");
		exit(1);
	}
	sleep(2);
	bzero(buf, sizeof(buf));
	if ((n = read(fd, buf, sizeof(buf))) < 0)
	{
		perror("read");
		exit(1);
	}

	printf("FIFO2 : Received a message: %s\n", buf);

	close(fd);
}

