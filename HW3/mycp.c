#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h> // exit() 경고 처리

#define	MAX_BUF	1024

int main(int argc, char *argv[])
{
	int 	fd1, fd2, count;
	char	buf[MAX_BUF];

	if (argc != 3)  {
		printf("Usage: %s source destination\n", argv[0]);
		exit(1);
	}

	if ((fd1 = open(argv[1], O_RDONLY)) < 0)  {
		perror("open");
		exit(1);
	}

	if ((fd2 = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0644)) < 0)  {
		perror("open");
		exit(1);
	}

	while ((count = read(fd1, buf, MAX_BUF)))  { 
		// MAX_BUF 만큼 읽고, 정상적으로 읽은 byte를 count에 저장.
		write(fd2, buf, count); // write()를 사용해 읽은 byte만큼 fd2 파일 디스크립터가 가르키는 파일에 작성.
	}

	close(fd1);
	close(fd2);
}
