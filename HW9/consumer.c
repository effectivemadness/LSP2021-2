#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include "prodcons.h"

int	main()
{
	BoundedBufferType	*pBuf; // bounded buffer type pointer 변수
	int					shmid, i, data; // shared mem id, data 변수

	if ((shmid = shmget(SHM_KEY, SHM_SIZE, SHM_MODE)) < 0)  { // shared memory 구하기
		perror("shmget");
		exit(1);
	}
	if ((pBuf = (BoundedBufferType *)shmat(shmid, 0, 0)) == (void *) -1)  { // shared memory를 프로세스에 붙이기
		perror("shmat");
		exit(1);
	}

	srand(0x9999); // rand 초기화
	for (i = 0 ; i < NLOOPS ; i++)  {
		if (pBuf->counter == 0)  { // 버퍼의 길이가 0이면 대기
			printf("Consumer: Buffer empty. Waiting.....\n");
			while (pBuf->counter == 0) // busy waiting
				;
		}

		printf("Consumer: Consuming an item.....\n"); // 버퍼 내용 있으면 가져오기
		data = pBuf->buf[pBuf->out].data; // data 가져오기
		pBuf->out = (pBuf->out + 1) % MAX_BUF; // 한 칸 밀기
		pBuf->counter--; // 버퍼 길이 하나 줄이기

		usleep((rand()%100)*10000); // 랜덤시간동안 sleep
	}

	printf("Consumer: Consumed %d items.....\n", i); // 최종 소비 아이템 출력
	printf("Consumer: %d items in buffer.....\n", pBuf->counter);
}
