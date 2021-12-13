#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include "shm.h"
#include <stdlib.h>
#include <unistd.h>

int	main()
{
	int		shmid; // shared memory id 저장 변수
	char	*ptr, *pData; // 주소값 저장할 변수들 
	int		*pInt; // int형으로 변환해 저장할 포인터 변수


	/* SHM_KEY, SHM_SIZE, SHM_MODE in shm.h */
	if ((shmid = shmget(SHM_KEY, SHM_SIZE, SHM_MODE)) < 0)  { // shared memory 할당
		perror("shmget");
		exit(1);
	}
	if ((ptr = shmat(shmid, NULL, 0)) == (void *)-1)  { // shared memory 프로세스에 붙이기
		perror("shmat");
		exit(1);
	}

	pInt = (int *)ptr; // int형으로 바꿔 변수에 저장 
	pData = ptr + sizeof(int); // data 는 int 변수 1개 공간 뒤부터 작성
	sprintf(pData, "This is a request from %d.", getpid()); // 데이터 부분 작성
	*pInt = 1; // shared mem 공간에 가장 앞 부분 int 에 1로 저장 
	printf("Sent a request.....");

	while ((*pInt) == 1) // 다른 프로세스가 1이 아닌 다른 수로 바꿀때까지 loop
		;

	printf("Received reply: %s\n", pData); // 다른 프로세스가 작성한 reply 출력 

	/* Detach shared memory */
	if (shmdt(ptr) < 0)  {
		perror("shmdt"); // shared memory 프로세스에서 떼기
		exit(1);
	}
}
