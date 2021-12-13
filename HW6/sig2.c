#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>

void
SigUsrHandler(int signo) // signal 번호가 인자로 들어옴
{
	if (signo == SIGUSR1)  { // SIGUSR1일 경우 
		printf("Received a SIGUSR1 signal\n"); // SIGUSR1 출력
	} 
	else if (signo == SIGUSR2)  { // SIGUSR2인 경우
		printf("Received a SIGUSR2 signal\n"); // SIGUSR2 출력
	}
	else  { // 기타 다른 시그널일 경우 프로그램 종료
		printf("Received unknown signal\n");
		printf("Terminate this process\n");
		exit(0);
	}
}

int	main()
{
	if (signal(SIGUSR1, SigUsrHandler) == SIG_ERR)  { // SIGUSR1에 Signal handling 함수 등록
		perror("signal"); // signal handling 함수 등록 실패시 에러 출력
		exit(1);
	}

	if (signal(SIGUSR2, SigUsrHandler) == SIG_ERR)  { // SIGUSR2에 Signal handling 함수 등록
		perror("signal"); // signal handling 함수 등록 실패시 에러 출력
		exit(1);
	}

	for ( ; ; )
		pause(); // 무한루프 돌면서 pause -> 시그널 올때까지 sleep
}
