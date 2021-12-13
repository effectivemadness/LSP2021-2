#include <stdio.h>
#include "synclib.h"
#include <stdlib.h>

#define	NLOOPS	5


int	main()
{
	int		i;
	pid_t	pid; 

	TELL_WAIT(); // 파이프 생성
	if ((pid = fork()) < 0)  { // 자식 프로세스 생성
		perror("fork"); // 실패시 에러처리
		exit(1);
	}
	else if (pid > 0)  { // 부모 프로세스일 경우
		for (i = 0 ; i < NLOOPS ; i++)  { // 루프 돌며
			TELL_CHILD(); // 자식에게 값 보내기
			printf("Parent: Tell to child\n");
			printf("Parent: Wait for child to tell\n");
			WAIT_CHILD(); // 자식이 보내는 값 대기
		}
	}
	else  { // 자식 프로세스일 경우
		for (i = 0 ; i < NLOOPS ; i++)  { // 루프 돌며
			printf("Child: Wait for parent to tell\n");
			WAIT_PARENT(); // 부모가 보내는 값 대기
			TELL_PARENT(); // 부모에게 값 보내기
			printf("Child: Tell to parent\n");
		}
	}
}
