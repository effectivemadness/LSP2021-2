#include <stdio.h>
#include <signal.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

pthread_t	ThreadId[2]; // 생성한 tID 저장할 전역변수 선언

void
SigIntHandler(int signo)
{
	int		i;

	printf("Received a SIGINT signal by thread %lu\n", pthread_self()); // 시그널 받은 쓰레드 ID 출력
	printf("Terminate other threads: tid=%lu, %lu\n", ThreadId[0], ThreadId[1]); // 취소할 쓰레드 ID들 출력

	for (i = 0 ; i < 2 ; i++)  {
		/* Cancel execution of all threads */
		if (pthread_cancel(ThreadId[i]))  { // 저장된 쓰레드ID를 사용해 쓰레드 취소
			perror("pthread_cancel");// 에러 발생시 처리
			exit(1);
		}
	}
	for (i = 0 ; i < 2 ; i++)  {
		if (pthread_join(ThreadId[i], NULL))  { // 각 쓰레드들 종료 대기
			perror("pthread_join"); // 에러 발생시 처리
			exit(1);
		}
	}
	printf("Threads terminated: tid=%lu, %lu\n", ThreadId[0], ThreadId[1]); // 쓰레드 종료 확인

	exit(0);
}

void
Thread1(void *dummy) // t1
{
	printf("Thread %lu created.....\n", pthread_self());

	if (pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL))  { // thread 취소 가능하게 설정, NULL : 이전 cancelstate
		perror("pthread_setcancelstate"); // 에러 발생시 표시
		pthread_exit(NULL); // 에러시 쓰레드 종료
	}
	if (pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL))  { // 비동기 : 바로 종료, DEFERRED : 취소 지점 벗어나서 종료
		perror("pthread_setcanceltype"); // 에러 발생시 표시
		pthread_exit(NULL); // 에러시 쓰레드 종료
	}

	while (1) // 무한루프
		;
}

void
Thread2(void *dummy)
{
	printf("Thread %lu created.....\n", pthread_self());

	if (pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL))  { // thread 취소 가능하게 설정, NULL : 이전 cancelstate
		perror("pthread_setcancelstate"); // 에러 발생시 표시
		pthread_exit(NULL); // 에러시 쓰레드 종료
	}
	if (pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL))  { // 비동기 : 바로 종료, DEFERRED : 취소 지점 벗어나서 종료
		perror("pthread_setcanceltype"); // 에러 발생시 표시
		pthread_exit(NULL); // 에러시 쓰레드 종료
	}

	while (1) // 무한루프
		;
}

int	main()
{
	if (pthread_create(&ThreadId[0], NULL, (void *)Thread1, NULL) < 0)  { // 쓰레드 생성후 Tid를 배열에 저장
		perror("pthread_create"); // 에러 발생시 표시
		exit(1); // 종료
	}
	if (pthread_create(&ThreadId[1], NULL, (void *)Thread2, NULL) < 0)  { // 쓰레드 생성후 Tid를 배열에 저장
		perror("pthread_create"); // 에러 발생시 표시
		exit(1); // 종료
	}

	signal(SIGINT, SigIntHandler); // SIGINT 시그널에 핸들러 붙이기

	printf("Press ^C to quit\n");

	for ( ; ; )
		pause(); // 루프 돌면서 sleep
}
