#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include "prodcons.h"

int	main()
{
	BoundedBufferType	*pBuf; // 데이터 저장할 구조체 포인터 선언
	int					shmid, i, data;

	if ((shmid = shmget(SHM_KEY, SHM_SIZE, SHM_MODE)) < 0)  { // Shared memory 얻기
		perror("shmget");
		exit(1);
	}
	if ((pBuf = (BoundedBufferType *)shmat(shmid, 0, 0)) == (void *) -1)  { // Shared memory 프로세스에 붙이기
		perror("shmat");
		exit(1);
	}

	srand(0x8888); // random seed 할당
	for (i = 0 ; i < NLOOPS ; i++)  { // NLOOP 번 루프
		if (pBuf->counter == MAX_BUF)  { // 버퍼가 가득 차있는 경우
			printf("Producer: Buffer full. Waiting.....\n");
			while (pBuf->counter == MAX_BUF) // busy waiting 하며 버퍼 대기
				;
		}

		printf("Producer: Producing an item.....\n");
		data = (rand()%100)*10000; // 랜덤 데이터 생성
		pBuf->buf[pBuf->in].data = data; // 버퍼에 데이터 넣기
		pBuf->in = (pBuf->in + 1) % MAX_BUF; // 버퍼 데이터 넣을 포인터 증가
		pBuf->counter++; //버퍼 안 데이터 개수 증가

		usleep(data); // 랜덤시간만큼 프로세스 sleep
	}

	printf("Producer: Produced %d items.....\n", i); 

	sleep(2); // 2초 대기
	printf("Producer: %d items in buffer.....\n", pBuf->counter);
	
	if (shmctl(shmid, IPC_RMID, 0) < 0)  { // shared memory 삭제
		perror("shmctl");
		exit(1);
	}
}
