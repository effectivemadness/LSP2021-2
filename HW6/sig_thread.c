#include <stdio.h>
#include <signal.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

//#define	THREAD_MAIN

#define	THREAD_1
#define	THREAD_2
// 생성된 쓰레드에서 핸들러를 등록해도, 메인 쓰레드에서 시그널을 받아서 처리


void
SigIntHandler(int signo)
{
	printf("Received a SIGINT signal by thread %lu\n", pthread_self()); // 시그널을 받은 쓰레드 출력
	printf("Terminate this process\n");

	exit(0);
}

void
Thread1(void *dummy)
{
#ifdef	THREAD_1
	signal(SIGINT, SigIntHandler); // thread1에 시그널 핸들링 함수 등록
#endif

	while (1)
		;
}

void
Thread2(void *dummy)
{
#ifdef	THREAD_2
	signal(SIGINT, SigIntHandler); // thread2에 시그널 핸들링 함수 등록
#endif

	while (1)
		;
}

int	main()
{

	pthread_t	tid1, tid2; // 쓰레드 ID 변수 선언 

	if (pthread_create(&tid1, NULL, (void *)Thread1, NULL) < 0)  { // 쓰레드1 실행 및 에러 발생시 에러 출력
		perror("pthread_create");
		exit(1);
	}
	if (pthread_create(&tid2, NULL, (void *)Thread2, NULL) < 0)  { // 쓰레드2 실행 및 에러 발생시 에러 출력
		perror("pthread_create");
		exit(1);
	}
	printf("Create two threads: tid1=%lu, tid2=%lu\n", tid1, tid2); // 두 쓰레드 출력
	printf("Main thread: tid=%lu\n", pthread_self()); // 메인 쓰레드 출력

#ifdef	THREAD_MAIN
	signal(SIGINT, SigIntHandler); // 메인 쓰레드에 시그널 핸들링 함수 등록
#endif

	printf("Press ^C to quit\n");

	for ( ; ; )
		pause(); //무한루프로 sleep
}
