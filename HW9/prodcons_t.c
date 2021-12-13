#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/time.h>
#include "prodcons.h"

BoundedBufferType	Buf;

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
Producer(void *dummy) // producer thread function
{
	int		i, data;

	printf("Producer: Start.....\n");

	for (i = 0 ; i < NLOOPS ; i++)  { // NLOOPS 동안 루프
		if (Buf.counter == MAX_BUF)  { // 버퍼가 꽉 차있는 경우
			printf("Producer: Buffer full. Waiting.....\n");
			while (Buf.counter == MAX_BUF) // busy waiting 하면서 대기
				;
		}

		printf("Producer: Producing an item.....\n");
		data = (rand()%100)*10000; // 랜덤 데이터 생성
		Buf.buf[Buf.in].data = data; // 데이터 추가
		Buf.in = (Buf.in + 1) % MAX_BUF; // 데이터 추가 지점 하나 밀기
		Buf.counter++; // 버퍼 데이터 개수 증가

		ThreadUsleep(data); // 랜덤시간동안 쓰레드 sleep
	}

	printf("Producer: Produced %d items.....\n", i);
	printf("Producer: %d items in buffer.....\n", Buf.counter);
	
	pthread_exit(NULL); // thread 종료
}

void
Consumer(void *dummy) // consumer thread function
{
	int		i, data;

	printf("Consumer: Start.....\n");

	for (i = 0 ; i < NLOOPS ; i++)  { // NLOOPS 동안 루프
		if (Buf.counter == 0)  { // 버퍼가 비어있을 경우
			printf("Consumer: Buffer empty. Waiting.....\n");
			while (Buf.counter == 0) // busy waiting 하면서 대기
				;
		}

		printf("Consumer: Consuming an item.....\n");
		data = Buf.buf[Buf.out].data; // 데이터 가져오기
		Buf.out = (Buf.out + 1) % MAX_BUF; // 데이터 소비 지점 하나 밀기
		Buf.counter--; // 버퍼 데이터 개수 감소

		ThreadUsleep((rand()%100)*10000); // 랜덤시간동안 쓰레드 sleep
	}

	printf("Consumer: Consumed %d items.....\n", i);
	printf("Consumer: %d items in buffer.....\n", Buf.counter);

	pthread_exit(NULL);
}

int	main()
{
	pthread_t	tid1, tid2;

	srand(0x8888);

	if (pthread_create(&tid1, NULL, (void *)Producer, (void *)NULL) < 0)  { // Producer 쓰레드 생성
		perror("pthread_create");
		exit(1);
	}

	if (pthread_create(&tid2, NULL, (void *)Consumer, (void *)NULL) < 0)  { // Consumer 쓰레드 생성
		perror("pthread_create");
		exit(1);
	}

	if (pthread_join(tid1, NULL) < 0)  { // Producer 쓰레드 종료 대기
		perror("pthread_join");
		exit(1);
	}
	if (pthread_join(tid2, NULL) < 0)  { // Consumer 쓰레드 종료 대기
		perror("pthread_join");
		exit(1);
	}

	printf("Main    : %d items in buffer.....\n", Buf.counter); // 최종 실행결과 출력
}
