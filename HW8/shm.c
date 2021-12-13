#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdlib.h>
#define	ARRAY_SIZE	40000
#define	MALLOC_SIZE	100000
#define	SHM_SIZE	100000
#define	SHM_MODE	(SHM_R | SHM_W)

char	Array[ARRAY_SIZE];

int	main()
{
	int		shmid; // shared memory id 저장 변수
	char	*ptr, *shmptr; // shared memroy 주소 및 malloc 주소 저장 변수

	if ((ptr = (char *)malloc(MALLOC_SIZE)) == NULL)  { // 동적할당
		perror("malloc");
		exit(1);
	}

	if ((shmid = shmget(IPC_PRIVATE, SHM_SIZE, SHM_MODE)) < 0)  { // shared memory 할당
		perror("shmget");
		exit(1);
	}
	if ((shmptr = shmat(shmid, 0, 0)) == (void *) -1)  { // 프로세스에 공유메모리 세그먼트 붙이기
		perror("shmat");
		exit(1);
	}

	printf("Array[] from %p to %p\n", &Array[0], &Array[ARRAY_SIZE]);
	printf("Stack around %p\n", &shmid);
	printf("Malloced from %p to %p\n", ptr, ptr+MALLOC_SIZE);
	printf("Shared memory attached from %p to %p\n", shmptr, shmptr+SHM_SIZE);

	
	if (shmdt(shmptr) < 0)  { // 프로세스에서 공유메모리 세그먼트 떼기
		perror("shmdt");
		exit(1);
	}

	if (shmctl(shmid, IPC_RMID, 0) < 0)  { // 공유메모리 할당 해제
		perror("shmctl");
		exit(1);
	}
}
