#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/time.h>
#include "semlib2.h" // semaphore.h가 아닌 semlib2.c 세마포어 구현체 사용
#include "prodcons.h"

BoundedBufferType	Buf; // shared memory 로 사용할 버퍼 선언
sem_t				EmptySem, FullSem, MutexSem; // 뮤텍스 선언

// thread Sleep 하는 함수
void
ThreadUsleep(int usecs)
{
	pthread_cond_t		cond; // condition variable 선언
	pthread_mutex_t		mutex; // mutex 선언
	struct timespec		ts; // 시간 변수들 선언
	struct timeval		tv;

	if (pthread_cond_init(&cond, NULL) < 0)  { // condition variable 초기화
		perror("pthread_cond_init"); // 에러처리
		pthread_exit(NULL);
	}
	if (pthread_mutex_init(&mutex, NULL) < 0)  { // mutex 초기화
		perror("pthread_mutex_init"); // 에러처리
		pthread_exit(NULL);
	}

	gettimeofday(&tv, NULL); // 현재 시간 가져오기
	ts.tv_sec = tv.tv_sec + usecs/1000000; // 초 단위 더하기
	ts.tv_nsec = (tv.tv_usec + (usecs%1000000)) * 1000; // 나노초 단위 더하기
	if (ts.tv_nsec >= 1000000000)  {
		ts.tv_nsec -= 1000000000;
		ts.tv_sec++;
	}

	if (pthread_mutex_lock(&mutex) < 0)  { // mutex lock 설정
		perror("pthread_mutex_lock");
		pthread_exit(NULL);
	}
	if (pthread_cond_timedwait(&cond, &mutex, &ts) < 0)  { // ts에 설정된 시간에 도달하기 전까지 현재 쓰레드 block
			//cond_signal, cond_broadcast 로 condition varialbe 호출되면 unblock
		perror("pthread_cond_timedwait");
		pthread_exit(NULL);
	}

	if (pthread_cond_destroy(&cond) < 0)  { // condition variable 제거
		perror("pthread_cond_destroy");
		pthread_exit(NULL);
	}
	if (pthread_mutex_destroy(&mutex) < 0)  { // mutex variable 제거
		perror("pthread_mutex_destroy");
		pthread_exit(NULL);
	}
}

void
Producer(void *dummy) // producer 쓰레드 함수
{
	int		i, data;

	printf("Producer: Start.....\n");

	for (i = 0 ; i < NLOOPS ; i++)  { // NLOOPS 루프
		if (sem_wait(&EmptySem) < 0)  { // 비어있는지 확인 -> 가득 차있으면 대기
			perror("sem_wait");
			pthread_exit(NULL);
		}
		if (sem_wait(&MutexSem) < 0)  { // shared memory(전역변수) 사용중인지 확인 -> 사용중이면 대기
			perror("sem_wait");
			pthread_exit(NULL);
		}

		printf("Producer: Producing an item.....\n");
		data = (rand()%100)*10000; // 랜덤 데이터 생성 
		Buf.buf[Buf.in].data = data; // 버퍼 in 지점에 데이터 저장 
		Buf.in = (Buf.in + 1) % MAX_BUF; // 버퍼 in 지점 한칸 증가
		Buf.counter++; // 데이터 개수 증가

		if (sem_post(&MutexSem) < 0)  { // shared mem 사용 종료 post
			perror("sem_post");
			pthread_exit(NULL);
		}
		if (sem_post(&FullSem) < 0)  { // 버퍼 비어있지 않음 세마포어 post
			perror("sem_post");
			pthread_exit(NULL);
		}

		ThreadUsleep(data); // 랜덤시간 sleep
	}

	printf("Producer: Produced %d items.....\n", i);
	printf("Producer: %d items in buffer.....\n", Buf.counter); 
	
	pthread_exit(NULL); // 쓰레드 종료
}

void
Consumer(void *dummy) // consumer 쓰레드 함수
{
	int		i, data;

	printf("Consumer: Start.....\n");

	for (i = 0 ; i < NLOOPS ; i++)  { // NLOOPS 동안
		if (sem_wait(&FullSem) < 0)  { // 비어있지 않는지 확인 -> 비어있으면 대기
			perror("sem_wait");
			pthread_exit(NULL);
		}
		if (sem_wait(&MutexSem) < 0)  { // shared mem 사용중인지 확인 -> 사용중이면 대기
			perror("sem_wait");
			pthread_exit(NULL);
		}

		printf("Consumer: Consuming an item.....\n");
		data = Buf.buf[Buf.out].data; // 버퍼 out 지점에서 데이터 가져오기
		Buf.out = (Buf.out + 1) % MAX_BUF; // 버퍼 out 지점 한칸 증가
		Buf.counter--; // 데이터 개수 감소

		if (sem_post(&MutexSem) < 0)  { // shared mem 사용 종료 post
			perror("sem_post");
			pthread_exit(NULL);
		}
		if (sem_post(&EmptySem) < 0)  { // 버퍼 빈 공간 세마포어 1 증가
			perror("sem_post");
			pthread_exit(NULL);
		}

		ThreadUsleep((rand()%100)*10000); // 랜덤시간 sleep
	}

	printf("Consumer: Consumed %d items.....\n", i);
	printf("Consumer: %d items in buffer.....\n", Buf.counter);

	pthread_exit(NULL); // 쓰레드 종료
}

int	main()
{
	pthread_t	tid1, tid2; // 두 쓰레드 id 선언

	srand(0x8888); // 랜덤seed

	if (sem_init(&EmptySem, 0, MAX_BUF) < 0)  { // 빈 공간 수 세마포어 선언 - 초기값 MAX_BUF
		perror("sem_init");
		exit(1);
	}
	if (sem_init(&FullSem, 0, 0) < 0)  { // 버퍼 비어있지 않음 세마포어 선언 - 초기값 0
		perror("sem_init");
		exit(1);
	}
	if (sem_init(&MutexSem, 0, 1) < 0)  { // shared memroy mutex 세마포어 선언 - 초기값 1
		perror("sem_init");
		exit(1);
	}

	if (pthread_create(&tid1, NULL, (void *)Producer, (void *)NULL) < 0)  { // producer 쓰레드 실행
		perror("pthread_create");
		exit(1);
	}

	if (pthread_create(&tid2, NULL, (void *)Consumer, (void *)NULL) < 0)  { // consumer 쓰레드 실행
		perror("pthread_create");
		exit(1);
	}

	//쓰레드 종료 대기 및 join
	if (pthread_join(tid1, NULL) < 0)  {
		perror("pthread_join");
		exit(1);
	}
	if (pthread_join(tid2, NULL) < 0)  {
		perror("pthread_join");
		exit(1);
	}

	printf("Main    : %d items in buffer.....\n", Buf.counter);
	if (sem_destroy(&EmptySem) < 0)  { // 빈 공간 수 세마포어 제거
		perror("sem_destroy");
	}
	if (sem_destroy(&FullSem) < 0)  { // 버퍼 비어있지 않음 세마포어 제거
		perror("sem_destroy");
	}
	if (sem_destroy(&MutexSem) < 0)  { // shared memory mutex 세마포어 제거
		perror("sem_destroy");
	}
}
