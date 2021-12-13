#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include "semlib.h"
#include "prodcons.h"

int	main()
{
	BoundedBufferType	*pBuf; // 버퍼 선언
	int					shmid, i, data;
	int					emptySemid, fullSemid, mutexSemid; //세마포어 id들 선언

	if ((shmid = shmget(SHM_KEY, SHM_SIZE, SHM_MODE)) < 0)  { // shared memory로 사용할 버퍼 생성
		perror("shmget");
		exit(1);
	}
	if ((pBuf = (BoundedBufferType *)shmat(shmid, 0, 0)) == (void *) -1)  { // 프로세스에 shared memory 붙이기
		perror("shmat");
		exit(1);
	}

	if ((emptySemid = semInit(EMPTY_SEM_KEY)) < 0)  { // 빈 공간 개수 세마포어 초기화
		fprintf(stderr, "semInit failure\n");
		exit(1);
	}
	if ((fullSemid = semInit(FULL_SEM_KEY)) < 0)  { // 버퍼 공간 차있음 세마포어 초기화
		fprintf(stderr, "semInit failure\n");
		exit(1);
	}
	if ((mutexSemid = semInit(MUTEX_SEM_KEY)) < 0)  { // shared memory 접근 뮤텍스 세마포어 초기화
		fprintf(stderr, "semInit failure\n");
		exit(1);
	}

	srand(0x8888); // 랜덤 Seed 설정
	for (i = 0 ; i < NLOOPS ; i++)  {
		if (semWait(emptySemid) < 0)  { // 비어있는지 확인 -> 비어있지 않으면 wait
			fprintf(stderr, "semWait failure\n");
			exit(1);
		}
		if (semWait(mutexSemid) < 0)  { // shared memory 사용중인지 확인 -> 사용중이면 wait
			fprintf(stderr, "semWait failure\n");
			exit(1);
		}

		printf("Producer: Producing an item.....\n");
		data = (rand()%100)*10000; // 랜덤 데이터 생성 
		pBuf->buf[pBuf->in].data = data; // 버퍼에 데이터 삽입
		pBuf->in = (pBuf->in + 1) % MAX_BUF; // 버퍼 데이터 들어갈 시점 증가
		pBuf->counter++; // 데이터 갯수 증가

		if (semPost(mutexSemid) < 0)  { // shared mem 사용중 세마포어 post
			fprintf(stderr, "semPost failure\n");
			exit(1);
		}
		if (semPost(fullSemid) < 0)  { // 버퍼에 데이터 있음 세마포어 post
			fprintf(stderr, "semPost failure\n");
			exit(1);
		}
		usleep(data); // 랜덤시간동안 대기
	}

	printf("Producer: Produced %d items.....\n", i);

	sleep(2); // 2초 대기
	printf("Producer: %d items in buffer.....\n", pBuf->counter);
	
	if (semDestroy(emptySemid) < 0)  { // 빈 공간 세마포어 삭제
		fprintf(stderr, "semDestroy failure\n");
	}
	if (semDestroy(fullSemid) < 0)  { // 버퍼 공간 차있음 세마포어 삭제
		fprintf(stderr, "semDestroy failure\n");
	}
	if (semDestroy(mutexSemid) < 0)  { // shared memory 접근 뮤텍스 세마포어 삭제
		fprintf(stderr, "semDestroy failure\n");
	}
	if (shmctl(shmid, IPC_RMID, 0) < 0)  { // 버퍼로 사용한 shared memory 삭제
		perror("shmctl");
		exit(1);
	}
}
