#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

void
SigIntHandler(int signo) // 시그널 핸들링 함수
{
	printf("Received a SIGINT signal\n");
	printf("Terminate this process\n");

	exit(0);
}

int	main()
{
	signal(SIGINT, SigIntHandler); // SIGINT 시그널에 SigIntHandler 함수를 핸들러로 붙이기

	printf("Press ^C to quit\n");

	for ( ; ; )
		pause(); // 시그널에 올때까지 프로그램을 sleep하기
}
