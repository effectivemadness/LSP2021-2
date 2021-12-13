#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/time.h>
#include "prodcons.h"

BoundedBufferType	Buf;
pthread_cond_t		NotFull;
pthread_cond_t		NotEmpty;
pthread_mutex_t		Mutex;
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

void // producer 쓰레드 함수
Producer(void *dummy)
{
	int		i, data;

	printf("Producer: Start.....\n");

	for (i = 0 ; i < NLOOPS ; i++)	{ // NLOOPS 번 루프
		if (pthread_mutex_lock(&Mutex) < 0)  { // mutex lock -> critical section 보호
			perror("pthread_mutex_lock");
			pthread_exit(NULL);
		}

		while (Buf.counter == MAX_BUF)  { // 버퍼가 가득 차있는 동안
			if (pthread_cond_wait(&NotFull, &Mutex) < 0)  { // NotFull 시그널 대기 -> 이 기간에 mutex lock 하는 쓰레드 있으면 여기서 풀어주고 그쪽 실행
				perror("pthread_cond_wait");
				pthread_exit(NULL);
			}
		}

		printf("Producer: Producing an item.....\n");
		data = (rand()%100)*10000; // 랜덤 데이터 생성
		Buf.buf[Buf.in].data = data; // 데이터 넣기 
		Buf.in = (Buf.in + 1) % MAX_BUF; // 데이터 삽입 위치 증가
		Buf.counter++; // 데이터 개수 증가

		if (pthread_cond_signal(&NotEmpty) < 0)  { // 데이터 넣었으므로 NotEmpty 시그널 보내기 
			perror("pthread_cond_signal");
			pthread_exit(NULL);
		}
		if (pthread_mutex_unlock(&Mutex) < 0)  { // critical secotion 벗어남 표시 -> mutex unlock
			perror("pthread_mutex_unlock");
			pthread_exit(NULL);
		}

		ThreadUsleep(data); // 랜덤시간 sleep
	}

	printf("Producer: Produced %d items.....\n", i);
	printf("Producer: %d items in buffer.....\n", Buf.counter);
	
	pthread_exit(NULL); // 쓰레드 종료
}

void
Consumer(void *dummy) // consumer 쓰레드
{
	int		i, data;

	printf("Consumer: Start.....\n");

	for (i = 0 ; i < NLOOPS ; i++)  { // NLOOPS 번 루프
		if (pthread_mutex_lock(&Mutex) < 0)  { // critical section 보호 -> mutex lock
			perror("pthread_mutex_lock");
			pthread_exit(NULL);
		}

		while (Buf.counter == 0)  { // 버퍼 비어있는동안
			if (pthread_cond_wait(&NotEmpty, &Mutex) < 0)  { // NotEmpty 시그널 대기 -> 이 기간에 mutex lock 하는 쓰레드 있으면 여기서 풀어주고 그쪽 쓰레드에서 실행
				perror("pthread_cond_wait");
				pthread_exit(NULL);
			}
		}

		printf("Consumer: Consuming an item.....\n");
		data = Buf.buf[Buf.out].data; // 데이터 가져오기
		Buf.out = (Buf.out + 1) % MAX_BUF; // 데이터 가져오는 위치 증가
		Buf.counter--; // 데이터 개수 감소

		if (pthread_cond_signal(&NotFull) < 0)  { // 데이터 하나 뺐으므로 NotFull 시그널 보내기
			perror("pthread_cond_signal");
			pthread_exit(NULL);
		}
		if (pthread_mutex_unlock(&Mutex) < 0)  { // critical section 벗어남 표시 -> mutex unlock
			perror("pthread_mutex_unlock");
			pthread_exit(NULL);
		}

		ThreadUsleep((rand()%100)*10000); // 랜덤시간 sleep
	}

	printf("Consumer: Consumed %d items.....\n", i);
	printf("Consumer: %d items in buffer.....\n", Buf.counter);

	pthread_exit(NULL); // 쓰레드 종료
}

int main()
{
	pthread_t	tid1, tid2; // 쓰레드 id

	srand(0x8888); // 랜덤 Seed
	
	if (pthread_cond_init(&NotFull, NULL) < 0)  { // condition var NotFull 생성
		perror("pthread_cond_init");
		pthread_exit(NULL);
	}
	if (pthread_cond_init(&NotEmpty, NULL) < 0)  { // condition var NotEmpty 생성
		perror("pthread_cond_init");
		pthread_exit(NULL);
	}
	if (pthread_mutex_init(&Mutex, NULL) < 0)  { // mutex 생성
		perror("pthread_mutex_init");
		pthread_exit(NULL);
	}

	if (pthread_create(&tid1, NULL, (void *)Producer, (void *)NULL) < 0)  { // producer 쓰레드 실행
		perror("pthread_create");
		exit(1);
	}

	if (pthread_create(&tid2, NULL, (void *)Consumer, (void *)NULL) < 0)  { // consumer 쓰레드 실행
		perror("pthread_create");
		exit(1);
	}

	// 쓰레드 종료 대기
	if (pthread_join(tid1, NULL) < 0)  {
		perror("pthread_join");
		exit(1);
	}
	if (pthread_join(tid2, NULL) < 0)  {
		perror("pthread_join");
		exit(1);
	}

	printf("Main    : %d items in buffer.....\n", Buf.counter);

	
	// Condition variable 제거
	if (pthread_cond_destroy(&NotFull) < 0)  {
		perror("pthread_cond_destroy");
	}
	if (pthread_cond_destroy(&NotEmpty) < 0)  {
		perror("pthread_cond_destroy");
	}
	// Mutex 제거
	if (pthread_mutex_destroy(&Mutex) < 0)  {
		perror("pthread_mutex_destroy");
	}
}
