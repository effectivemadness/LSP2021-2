#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>
#include <strings.h>

int main()
{
	int fd, n;
	char buf[10];
	if (mkfifo("myfifo", 0600) < 0){
		perror("mkfifo");
		exit(1);
	}

	if ((fd = open("myfifo", O_RDWR)) < 0)
	{
		perror("open");
		exit(1);
	}
	bzero(buf, sizeof(buf));
	if ((n = read(fd, buf, sizeof(buf))) < 0)
	{
		perror("read");
		exit(1);
	}
	printf("FIFO1 : Received a message: %s\n", buf);
	bzero(buf, sizeof(buf));
	strcpy(buf, "Hi");
	if (write(fd, buf, strlen(buf)) < 0)
	{
		perror("write");
		exit(1);
	}

	printf("FIFO1 : Sent a message: %s\n", buf);

	close(fd);
	if (remove("myfifo") < 0)
	{
		perror("remove");
		exit(1);
	}
}

