#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include "shm.h"
#include <stdlib.h>
#include <unistd.h>
#include "semlib.h"

/*===============================================================
[Program Name] : sipc2
[Description]  : 
    - sipc1 프로그램과 shared memory를 사용해 데이터를 주고받는다
	- 두 프로세스간 동기화는 세마포어를 사용해 구현한다.
[Input]        :
    -
[Output]       :
    shell stdout로 주고받은 스트링을 출력한다.
[Calls]        :	        
    -
[특기사항]     : 두 세마포어(sem1id, sem2id)를 사용해 각 프로세스 실행 순서를 동기화한다.
==================================================================*/
int	main()
{
	int		shmid, sem1id, sem2id; // shared memory id 저장 변수, 세마포어 id
	char	*ptr, *pData; // 주소값 저장할 변수들 


	/* SHM_KEY, SHM_SIZE, SHM_MODE in shm.h */
	if ((shmid = shmget(SHM_KEY, SHM_SIZE, SHM_MODE)) < 0)  { // shared memory 할당
		perror("shmget");
		exit(1);
	}
	if ((ptr = shmat(shmid, NULL, 0)) == (void *)-1)  { // shared memory 프로세스에 붙이기
		perror("shmat");
		exit(1);
	}

	if ((sem2id = semInit(SEM_2_KEY)) < 0)  { // sipc2 작성 차례 세마포어 초기화
		fprintf(stderr, "semInit failure\n");
		exit(1);
	}
	if ((sem1id = semInit(SEM_1_KEY)) < 0)  { // sipc1 작성 차례 세마포어 초기화
		fprintf(stderr, "semInit failure\n");
		exit(1);
	}
	if (semInitValue(sem2id, 1) < 0)  { // sipc2부터 작성
		fprintf(stderr, "semInitValue failure\n"); 
		exit(1);
	}
	if (semInitValue(sem1id, 0) < 0)  { // sipc1은 대기상태로 시작
		fprintf(stderr, "semInitValue failure\n"); 
		exit(1);
	}

	if (semWait(sem2id) < 0)  { // sipc2 작성차례 대기
			fprintf(stderr, "semWait failure\n");
			exit(1);
	}
	pData = ptr; // 전체 데이터 공간 사용
	
	sprintf(pData, "This is a request from %d.", getpid()); // 데이터 부분 작성
	printf("Sent a request.....");

	if (semPost(sem1id) < 0)  { // sipc1 접근 차례 post
			fprintf(stderr, "semPost failure\n");
			exit(1);
	}

	if (semWait(sem2id) < 0)  { // sipc2 접근 차례 대기
			fprintf(stderr, "semWait failure\n");
			exit(1);
	}
	printf("Received reply: %s\n", pData); // 다른 프로세스가 작성한 reply 출력 

	/* Detach shared memory */
	if (shmdt(ptr) < 0)  {
		perror("shmdt"); // shared memory 프로세스에서 떼기
		exit(1);
	}
}
