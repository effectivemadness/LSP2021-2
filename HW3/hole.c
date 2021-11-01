#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

int main()
{
	int 	fd;
	char	ch = '\0';

	if ((fd = creat("file.hole", 0400)) < 0)  { // 새 파일 생성
		perror("creat");
		exit(1);
	}

	if (lseek(fd, 1023, SEEK_SET) < 0)  { // 파일 내부 위치 이동
		perror("lseek");
		exit(1);
	}
	write(fd, &ch, 1); // 그 자리에 널 문자 작성

	close(fd); //파일 닫기
}
