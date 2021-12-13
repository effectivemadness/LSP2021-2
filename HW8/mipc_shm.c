#include <stdio.h>
#include <sys/types.h>
#include "synclib.h"
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include "shm.h"

#define	NLOOPS	10
#define	SIZE	sizeof(long)


int
update(int *ptr) // 포인터 받아서 포인터가 가르키는 값 증가
{
	return ((*ptr)++);
}
/*===============================================================
[Program Name] : mipc_shm
[Description]  : 
    - 부모와 자식간 통신을 shared memory로 구현해 본다
[Input]        :
	-
[Output]       :
    - 쉘 표준 출력으로 부모와 자식이 번갈아가며 출력한다
[Calls]        :	        
    int update(int *ptr) // 주소값을 입력받아, 해당 값을 증가시키고 증가시키기 전 값 리턴
[특기사항]     : mipc 실습 코드를 수정해 구현하였다.
==================================================================*/
int	main()
{
	int		i, counter; // file descriptor, for i, counter 선언
	pid_t	pid;
	int     shmid; // 공유 메모리 id
	char    *ptr; // 주소를 저장할 변수
	int     *pInt; // int 타입 포인터 변수

	if ((shmid = shmget(SHM_KEY, SHM_SIZE, SHM_MODE)) < 0)  { // shared memory 할당	
		perror("shmget");
		exit(1);
	}
	if ((ptr = shmat(shmid, 0, 0)) == (void *) - 1) // shared memory 프로세스에 붙이기
	{
		perror("shmat");
		exit(1);
	}
	
	pInt = (int *)ptr; // shared memory 를 int*형으로 바꿔 저장
	*pInt = 0; // 0으로 초기화
	TELL_WAIT(); // fd 2개 선언, 부모자식간 통신 가능한 파이프 설정
	if ((pid = fork()) < 0)  { // fork 진행 - 분기
		perror("fork");
		exit(1);
	}
	else if (pid > 0)  { // 부모일 경우.
		for (i = 0 ; i < NLOOPS ; i += 2)  { // 0부터 짝수부분 실행 
			if ((counter = update( pInt)) != i)  { // shared memory 값에 1 증가시킨게 현재 i와 다르면 카운터 에러
				fprintf(stderr, "Counter mismatch\n");
				exit(1);
			}
			printf("Parent: counter=%d\n", counter); // 현재 카운터 출력 
			TELL_CHILD(); // 자식에게 pipe 로 값 전달
			WAIT_CHILD(); // 자식 대기
		}

		wait(); // 자식 종료 대기
		if (shmctl(shmid, IPC_RMID, 0) < 0) // shared memory 제거
		{		
			perror("shmctl");
			exit(1);
		}
	}
	else  { // 자식일 경우
		for (i = 1 ; i < NLOOPS ; i += 2)  { // 1부터 홀수부분 실행 
			WAIT_PARENT(); // 부모 대기
			if ((counter = update(pInt)) != i)  { // shared memory 값에 1 증가시킨게 현재 i와 다르면 카운터 에러
				fprintf(stderr, "Counter mismatch\n");
				exit(1);
			}
			printf("Child : counter=%d\n", counter); // 현재 카운터 출력
			TELL_PARENT(); // 부모에게 pipe로 값 전달
		}
	}

}
