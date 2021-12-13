#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/time.h>
#include "prodcons.h"
#include "semlib2.h" // semaphore.h가 아닌 semlib2.c 세마포어 구현체 사용

/*
SEMAPHORE와 mutex+condition variable
	Semaphore의 counting 개념이 mutex+cond var와 가장 다른 부분이다.
	condition variable은, wait하고 있는 쓰레드/프로세스에 신호를 주어 실행할 수 있게 한다.
	만약 wait하고 있는 쓰레드/프로세스가 없으면 아무 영향이 없다.
	Semaphore의 경우, post시 무조건 세마포어의 값이 1 증가하므로, 꼭 binary가 아닌 값을 가질 수 있다.
*/

// 아래 줄 주석처리시 mutex+condition variable 구현체
// #define SEMAPHORE

int					dir;
char				buf[1024];

#ifndef SEMAPHORE
pthread_cond_t		sipc1_cond;
pthread_cond_t		sipc2_cond;
pthread_mutex_t		Mutex;
#endif

#ifdef SEMAPHORE
sem_t				sipc1Sem, sipc2Sem, MutexSem; // 세마포어들 선언
#endif



/*===============================================================
[Function Name] : void	sipc1(void* dummy)
[Description]   : 
    - sipc2 쓰레드와 전역함수 버퍼를 사용해 데이터를 주고받는다
[Input]         :
    void *dummy;	// 아무 영향없는 인자
[Output]        :
    shell stdout로 주고받은 스트링 출력
[Call By]       :	        
    Main()
[Calls]         :	        
    Nothing
[Given]         :
    Nothing
[Returns]       :
    Nothing
==================================================================*/
void
sipc1(void *dummy)
{
	int		i, data;

	printf("sipc1: Start.....\n");

	#ifndef SEMAPHORE
	if (pthread_mutex_lock(&Mutex) < 0)  { // mutex lock -> critical section 보호
		perror("pthread_mutex_lock");
		pthread_exit(NULL);
	}

	while (dir != 1)  { // dir 다르면
		if (pthread_cond_wait(&sipc1_cond, &Mutex) < 0)  { // sipc1_cond 시그널 대기 -> 이 기간에 mutex lock 하는 쓰레드 있으면 여기서 풀어주고 그쪽 실행
			perror("pthread_cond_wait");
			pthread_exit(NULL);
		}
	}
	#endif

	#ifdef SEMAPHORE
	if (sem_wait(&sipc1Sem) < 0)  { // sipc1 순서인지 확인 -> 아니면 대기
			perror("sem_wait");
			pthread_exit(NULL);
		}
	if (sem_wait(&MutexSem) < 0)  { // buf 사용 mutex 대기
		perror("sem_wait");
		pthread_exit(NULL);
	}
	#endif
	printf("Received request: %s.....", buf); // data 출력
	sprintf(buf, "This is a reply from %ld.", pthread_self()); // data에 작성
	printf("Replied.\n");
	dir = 2;
	#ifndef SEMAPHORE
	if (pthread_cond_signal(&sipc2_cond) < 0)  { // 데이터 넣었으므로 sipc2_cond 시그널 보내기 
		perror("pthread_cond_signal");
		pthread_exit(NULL);
	}
	if (pthread_mutex_unlock(&Mutex) < 0)  { // critical secotion 벗어남 표시 -> mutex unlock
		perror("pthread_mutex_unlock");
		pthread_exit(NULL);
	}
	#endif
	#ifdef SEMAPHORE
	if (sem_post(&MutexSem) < 0)  { // 버퍼 사용 종료 post
			perror("sem_post");
			pthread_exit(NULL);
		}
	if (sem_post(&sipc2Sem) < 0)  { // sipc2 순서 post
		perror("sem_post");
		pthread_exit(NULL);
	}
	#endif

	
	pthread_exit(NULL); // 쓰레드 종료
}

/*===============================================================
[Function Name] : void	sipc2(void* dummy)
[Description]   : 
    - sipc1 쓰레드와 전역함수 버퍼를 사용해 데이터를 주고받는다
[Input]         :
    void *dummy;	// 아무 영향없는 인자
[Output]        :
    shell stdout로 주고받은 스트링 출력
[Call By]       :	        
    Main()
[Calls]         :	        
    Nothing
[Given]         :
    Nothing
[Returns]       :
    Nothing
==================================================================*/
void
sipc2(void *dummy) // sipc2 쓰레드
{
	int		i, data;

	printf("sipc2: Start.....\n");
	#ifndef SEMAPHORE
	if (pthread_mutex_lock(&Mutex) < 0)  { // critical section 보호 -> mutex lock
		perror("pthread_mutex_lock");
		pthread_exit(NULL);
	}

	while (dir != 2)  { // dir 다르면
		if (pthread_cond_wait(&sipc2_cond, &Mutex) < 0)  { // sipc2_cond 시그널 대기 -> 이 기간에 mutex lock 하는 쓰레드 있으면 여기서 풀어주고 그쪽 쓰레드에서 실행
			perror("pthread_cond_wait");
			pthread_exit(NULL);
		}
	}
	#endif

	#ifdef SEMAPHORE
	if (sem_wait(&sipc2Sem) < 0)  { // sipc2 순서 대기
			perror("sem_wait");
			pthread_exit(NULL);
		}
	if (sem_wait(&MutexSem) < 0)  { // 버퍼 사용중인지 확인 -> 사용중이면 대기
		perror("sem_wait");
		pthread_exit(NULL);
	}
	#endif

	sprintf(buf, "This is a request from %ld.", pthread_self()); // 데이터 부분 작성
	printf("Sent a request.....");
	dir = 1;
	#ifndef SEMAPHORE
	if (pthread_cond_signal(&sipc1_cond) < 0)  { // 데이터 작성했으므로 sipc1_cond 시그널 보내기
		perror("pthread_cond_signal");
		pthread_exit(NULL);
	}
	if (pthread_mutex_unlock(&Mutex) < 0)  { // critical section 벗어남 표시 -> mutex unlock
		perror("pthread_mutex_unlock");
		pthread_exit(NULL);
	}
	#endif
	#ifdef SEMAPHORE
	if (sem_post(&MutexSem) < 0)  { // 버퍼 사용 종료 post
			perror("sem_post");
			pthread_exit(NULL);
		}
	if (sem_post(&sipc1Sem) < 0)  { // sipc1 순서 post
		perror("sem_post");
		pthread_exit(NULL);
	}
	#endif
	#ifndef SEMAPHORE
	if (pthread_mutex_lock(&Mutex) < 0)  { // critical section 보호 -> mutex lock
		perror("pthread_mutex_lock");
		pthread_exit(NULL);
	}

	while (dir != 2)  { // dir 다르면
		if (pthread_cond_wait(&sipc2_cond, &Mutex) < 0)  { // sipc2_cond 시그널 대기 -> 이 기간에 mutex lock 하는 쓰레드 있으면 여기서 풀어주고 그쪽 쓰레드에서 실행
			perror("pthread_cond_wait");
			pthread_exit(NULL);
		}
	}
	#endif
	#ifdef SEMAPHORE
	if (sem_wait(&sipc2Sem) < 0)  { // sipc2 순서 대기
			perror("sem_wait");
			pthread_exit(NULL);
		}
	if (sem_wait(&MutexSem) < 0)  { // 버퍼 사용중인지 확인 -> 사용중이면 대기
		perror("sem_wait");
		pthread_exit(NULL);
	}
	#endif

	printf("Received reply: %s\n", buf); // 다른 프로세스가 작성한 reply 출력 

	pthread_exit(NULL); // 쓰레드 종료
}

/*===============================================================
[Program Name] : sipc_m
[Description]  : 
    - 두 쓰레드 간 global buffer를 사용해 데이터를 주고받는다.
	- 두 쓰레드 간 동기화는 세마포어와 mutex+condition variable을 사용해 구현한다.
[Input]        :
    -
[Output]       :
    shell stdout로 주고받은 스트링을 출력한다.
[Calls]        :	        
    -
[특기사항]     : 
	#define SEMAPHORE 플래그로 실행시 세마포어 구현체,
	#define SEMAPHORE 플래그 없이 실행시 mutex+condition variable 구현체로 실행된다.
==================================================================*/
int main()
{
	pthread_t	tid1, tid2; // 쓰레드 id

	#ifndef SEMAPHORE
	printf("mutex+condition variable\n");
	if (pthread_cond_init(&sipc1_cond, NULL) < 0)  { // condition var sipc1_cond 생성
		perror("pthread_cond_init");
		pthread_exit(NULL);
	}
	if (pthread_cond_init(&sipc2_cond, NULL) < 0)  { // condition var sipc2_cond 생성
		perror("pthread_cond_init");
		pthread_exit(NULL);
	}
	if (pthread_mutex_init(&Mutex, NULL) < 0)  { // mutex 생성
		perror("pthread_mutex_init");
		pthread_exit(NULL);
	}
	#endif
	#ifdef SEMAPHORE
	printf("semaphore\n");
	if (sem_init(&sipc1Sem, 0, 0) < 0)  { // sipc1 순서 세마포어 -> sipc1는 나중에 실행
		perror("sem_init");
		exit(1);
	}
	if (sem_init(&sipc2Sem, 0, 1) < 0)  { // sipc2 순서 세마포어 -> sipc2부터 실행
		perror("sem_init");
		exit(1);
	}
	if (sem_init(&MutexSem, 0, 1) < 0)  { // 공용 buffer mutex 세마포어 선언 - 초기값 1
		perror("sem_init");
		exit(1);
	}
	#endif
	dir = 2; // sipc2부터 작성
	if (pthread_create(&tid1, NULL, (void *)sipc1, (void *)NULL) < 0)  { // sipc1 쓰레드 실행
		perror("pthread_create");
		exit(1);
	}

	if (pthread_create(&tid2, NULL, (void *)sipc2, (void *)NULL) < 0)  { // sipc2 쓰레드 실행
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
	#ifndef SEMAPHORE
	// Condition variable 제거
	if (pthread_cond_destroy(&sipc1_cond) < 0)  {
		perror("pthread_cond_destroy");
	}
	if (pthread_cond_destroy(&sipc2_cond) < 0)  {
		perror("pthread_cond_destroy");
	}
	// Mutex 제거
	if (pthread_mutex_destroy(&Mutex) < 0)  {
		perror("pthread_mutex_destroy");
	}
	#endif
	#ifdef SEMAPHORE
	// 세마포어 제거
	if (sem_destroy(&sipc1Sem) < 0)  {
		perror("sem_destroy");
	}
	if (sem_destroy(&sipc2Sem) < 0)  {
		perror("sem_destroy");
	}
	if (sem_destroy(&MutexSem) < 0)  {
		perror("sem_destroy");
	}
	#endif
}
