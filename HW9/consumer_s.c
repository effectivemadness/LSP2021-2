#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include "semlib.h"
#include "prodcons.h"

int 	main()
{
	BoundedBufferType	*pBuf; // 버퍼로 사용할 메모리 포인터
	int					shmid, i, data; 
	int					emptySemid, fullSemid, mutexSemid; // 사용할 세마포어 id 들
 
	if ((shmid = shmget(SHM_KEY, SHM_SIZE, SHM_MODE)) < 0)  { // shared memory 할당
		perror("shmget");
		exit(1);
	}
	if ((pBuf = (BoundedBufferType *)shmat(shmid, 0, 0)) == (void *) -1)  { // 현재 프로세스에 shared memory 붙이기
		perror("shmat");
		exit(1);
	}

	if ((emptySemid = semInit(EMPTY_SEM_KEY)) < 0)  { // 빈 공간 세마포어 초기화
		fprintf(stderr, "semInit failure\n");
		exit(1);
	}
	if ((fullSemid = semInit(FULL_SEM_KEY)) < 0)  { // 버퍼 비어있지 않음  세마포어 초기화
		fprintf(stderr, "semInit failure\n");
		exit(1);
	}
	if ((mutexSemid = semInit(MUTEX_SEM_KEY)) < 0)  { // shared memory mutex 세마포어 초기화
		fprintf(stderr, "semInit failure\n");
		exit(1);
	}

	if (semInitValue(emptySemid, MAX_BUF) < 0)  { // 빈 공간 세마포어 버퍼크기로 설정
		fprintf(stderr, "semInitValue failure\n");
		exit(1);
	}
	if (semInitValue(fullSemid, 0) < 0)  { // 버퍼 비어있지 않음 세마포어 0으로 설정
		fprintf(stderr, "semInitValue failure\n"); 
		exit(1);
	}
	if (semInitValue(mutexSemid, 1) < 0)  { // shared memory mutex 세마포어 1로 초기화 -> 접근 가능
		fprintf(stderr, "semInitValue failure\n");
		exit(1);
	}

	srand(0x9999); // 랜덤 seed
	for (i = 0 ; i < NLOOPS ; i++)  {
		if (semWait(fullSemid) < 0)  { // 버퍼 비어있으면 대기
			fprintf(stderr, "semWait failure\n");
			exit(1);
		}
		if (semWait(mutexSemid) < 0)  { // shared memory 사용중이면 대기
			fprintf(stderr, "semWait failure\n");
			exit(1);
		}
		printf("Consumer: Consuming an item.....\n");
		data = pBuf->buf[pBuf->out].data; // 데이터 가져오기
		pBuf->out = (pBuf->out + 1) % MAX_BUF; // 소비지점 하나 증가
		pBuf->counter--; // 저장된 데이터 개수 감소

		if (semPost(mutexSemid) < 0)  { // shared memory 사용 끝 표시
			fprintf(stderr, "semPost failure\n");
			exit(1);
		}
		if (semPost(emptySemid) < 0)  { // 빈 공간 세마포어 증가
			fprintf(stderr, "semPost failure\n");
			exit(1);
		}

		usleep((rand()%100)*10000); // 랜덤 시간 sleep
	}

	printf("Consumer: Consumed %d items.....\n", i);
	printf("Consumer: %d items in buffer.....\n", pBuf->counter);
}
