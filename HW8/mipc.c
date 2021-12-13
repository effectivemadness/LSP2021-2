#include <stdio.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <fcntl.h>
#include "synclib.h"
#include <stdlib.h>
#include <unistd.h>

#define	NLOOPS	10
#define	SIZE	sizeof(long)


int
update(long *ptr) // 포인터 받아서 포인터가 가르키는 값 증가
{
	return ((*ptr)++);
}

int	main()
{
	int		fd, i, counter; // file descriptor, for i, counter 선언
	pid_t	pid;
	caddr_t	area;

	if ((fd = open("/dev/zero", O_RDWR)) < 0)  { // /dev/zero 파일을 열기
		perror("open");
		exit(1);
	}
	if ((area = mmap(0, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0)) // /dev/zero 파일을 memory mapped file로 만들기
		== (caddr_t) -1)  {
		perror("mmap");
		exit(1);
	}
	close(fd);

	TELL_WAIT(); // fd 2개 선언, 부모자식간 통신 가능한 파이프 설정
	if ((pid = fork()) < 0)  { // fork 진행 - 분기
		perror("fork");
		exit(1);
	}
	else if (pid > 0)  { // 부모일 경우.
		for (i = 0 ; i < NLOOPS ; i += 2)  { // 0부터 짝수부분 실행 
			if ((counter = update((long *) area)) != i)  { // memory mapped file 값에 1 증가시킨게 현재 i와 다르면 카운터 에러
				fprintf(stderr, "Counter mismatch\n");
				exit(1);
			}
			printf("Parent: counter=%d\n", counter); // 현재 카운터 출력 
			TELL_CHILD(); // 자식에게 pipe 로 값 전달
			WAIT_CHILD(); // 자식 대기
		}
	}
	else  { // 자식일 경우
		for (i = 1 ; i < NLOOPS ; i += 2)  { // 1부터 홀수부분 실행 
			WAIT_PARENT(); // 부모 대기
			if ((counter = update((long *) area)) != i)  { // memory mapped file 값에 1 증가시킨게 현재 i와 다르면 카운터 에러
				fprintf(stderr, "Counter mismatch\n");
				exit(1);
			}
			printf("Child : counter=%d\n", counter); // 현재 카운터 출력
			TELL_PARENT(); // 부모에게 pipe로 값 전달
		}
	}
}
