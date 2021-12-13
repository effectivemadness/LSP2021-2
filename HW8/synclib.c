#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>

static int	Pfd1[2], Pfd2[2]; // 통신할 2 세트의 파이프 변수 배열
//Pfd2[0] (p) <- (c) Pfd2[1]
//Pfd1[0] (c) <- (p) Pfd1[1]
void
TELL_WAIT(void) // 통신할 파이프 생성
{
	if (pipe(Pfd1) < 0 || pipe(Pfd2) < 0)  { // 생성 실패시 
		perror("pipe"); // 에러처리
		exit(1);
	}
}

void
TELL_PARENT(void) // 부모에게 보내기
{
	if (write(Pfd2[1], "c", 1) != 1)  { // 파이프에 작성
		perror("write"); // 실패시 에러처리
		exit(1);
	}
}

void
WAIT_PARENT(void) // 부모가 보내는 정보 받아오기
{
	char	c; // 저장할 c 변수

	if (read(Pfd1[0], &c, 1) != 1)  { // 부모가 보내는 파이프에서 읽어오기
		perror("read"); // 실패시 에러처리
		exit(1);
	}
	if (c != 'p')  { // 받아오는 정보가 틀렸을경우
		fprintf(stderr, "WAIT_PARENT: incorrect data"); // 에러처리
		exit(1);
	}
}

void
TELL_CHILD(void) // 자식에게 보내기
{
	if (write(Pfd1[1], "p", 1) != 1)  { // 파이프에 작성
		perror("write"); // 실패시 에러처리
		exit(1);
	}
}

void
WAIT_CHILD(void) // 자식이 보내는 정보 받아오기
{
	char	c; // 저장할 c 변수

	if (read(Pfd2[0], &c, 1) != 1)  { // 자식이 보내는 파이프에서 읽어오기
		perror("read"); // 실패시 에러처리
		exit(1);
	}
	if (c != 'c')  { // 받아오는 정보가 틀렸을경우
		fprintf(stderr, "WAIT_CHILD: incorrect data"); // 에러처리
		exit(1);
	}
}
