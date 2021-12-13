#include <stdio.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

int	main(int argc, char *argv[])
{
	int			fdin, fdout; // 입력, 출력 file descriptor
	char		*src, *dst; // 원본, 복제 할 파일을 가르킬 포인터 변수
	struct stat	statbuf; // statbuf 구조체

	if (argc != 3)  { // 인자 수 예외 처리
		fprintf(stderr, "Usage: %s source destination\n", argv[0]);
		exit(1);
	}

	if ((fdin = open(argv[1], O_RDONLY)) < 0)  { // source 파일 열기
		perror("open");
		exit(1);
	}
	if ((fdout = open(argv[2], O_RDWR | O_CREAT | O_TRUNC, 0644)) < 0)  { // dest 파일 생성
		perror("open");
		exit(1);
	}

	if (fstat(fdin, &statbuf) < 0)  { // source 파일의 stat 불러오기
		perror("fstat");
		exit(1);
	}
	if (lseek(fdout, statbuf.st_size-1, SEEK_SET) < 0)  { // fdout 파일을 탐색. source 길이만큼 이동
		perror("lseek");
		exit(1);
	}
	write(fdout, "", 1); // 빈 문자 작성

	if ((src = mmap(0, statbuf.st_size, PROT_READ, MAP_SHARED, fdin, 0)) // memory mapped file 생성, statbuf.st_size 길이만큼 fdin 파일이 메모리 공간에 매핑됨
		== (caddr_t) -1)  {
		perror("mmap");
		exit(1);
	}
	if ((dst = mmap(0, statbuf.st_size, PROT_WRITE, MAP_SHARED, fdout, 0)) // memory mapped file 생성, statbuf.st_size 길이만큼 fdout 파일이 메모리 공간에 매핑됨
		== (caddr_t) -1)  {
		perror("mmap");
		exit(1);
	}

	memcpy(dst, src, statbuf.st_size); // statbuf.st_size 길이만큼 메모리 복사

	close(fdin); // 파일들 닫기
	close(fdout);
}
