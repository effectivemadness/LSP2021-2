#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include "shm.h"
#include "semlib.h"
#include <stdlib.h>
#include <unistd.h>

/*===============================================================
[Program Name] : sipc1
[Description]  : 
    - sipc2 프로그램과 shared memory를 사용해 데이터를 주고받는다
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
	int		shmid, sem1id, sem2id; // 공유 메모리 id
	char	*ptr, *pData; // 주소를 저장할 변수

	/* SHM_KEY, SHM_SIZE, SHM_MODE in shm.h */
	if ((shmid = shmget(SHM_KEY, SHM_SIZE, SHM_MODE)) < 0)  { // shared memory 할당
		perror("shmget");
		exit(1);
	}

	if ((ptr = shmat(shmid, 0, 0)) == (void *) -1)  { // shared memory attach
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

	if (semWait(sem1id) < 0)  { // sipc1 접근 차례 대기
			fprintf(stderr, "semWait failure\n");
			exit(1);
		}
	
	pData = ptr; // 전체를 데이터로 사용
	printf("Received request: %s.....", pData); // data 출력
	sprintf(pData, "This is a reply from %d.", getpid()); // data에 작성
	
	printf("Replied.\n");

	if (semPost(sem2id) < 0)  { // sipc2 접근 차례 post
			fprintf(stderr, "semPost failure\n");
			exit(1);
	}
	sleep(1);

	/* Detach shared memory */
	if (shmdt(ptr) < 0)  { // shared memory 세그먼트 프로세스에서 떼기
		perror("shmdt");
		exit(1);
	}

	/* Remove shared memory */
	if (shmctl(shmid, IPC_RMID, NULL))  { // shared memory 할당 제거
		perror("shmctl");
		exit(1);
	}
	// 세마포어 제거
	if (semDestroy(sem1id) < 0)  { 
		fprintf(stderr, "semDestroy failure\n");
	}
	if (semDestroy(sem2id) < 0)  {
		fprintf(stderr, "semDestroy failure\n");
	}
}
