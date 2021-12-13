#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include "shm.h"
#include <stdlib.h>
#include <unistd.h>

int	main()
{
	int		shmid; // 공유 메모리 id
	char	*ptr, *pData; // 주소를 저장할 변수
	int		*pInt; // int 타입 포인터 변수

	/* SHM_KEY, SHM_SIZE, SHM_MODE in shm.h */
	if ((shmid = shmget(SHM_KEY, SHM_SIZE, SHM_MODE)) < 0)  { // shared memory 할당
		perror("shmget");
		exit(1);
	}
	if ((ptr = shmat(shmid, 0, 0)) == (void *) -1)  { // shared memory attach
		perror("shmat");
		exit(1);
	}

	pInt = (int *)ptr; // shared mem 주소를 int형으로 쓸 변수에 저장
	while ((*pInt) == 0) // 초기값인 경우 무한루프
		;

	pData = ptr + sizeof(int); // data는 int 변수 1개 다음부터 
	printf("Received request: %s.....", pData); // data 출력
	sprintf(pData, "This is a reply from %d.", getpid()); // data에 작성
	*pInt = 0; // 다시 0으로 되돌리기
	printf("Replied.\n");

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
}
