#include <stdio.h>
#include <signal.h>
#include <unistd.h>

void
SigAlarmHandler(int signo)
{
	/* nothing to do, just return to wake up the pause */
	// 핸들링 함수 실행되고 바로 리턴 -> sleep 만 해제.
	return;
}

unsigned int
mysleep(unsigned int nsecs)
{
	if (signal(SIGALRM, SigAlarmHandler) == SIG_ERR)  { // SIGALRM에 SigAlarmHandler 핸들링 함수 붙이기
		return nsecs; // 실패시 해당 시간 리턴
	}

	alarm(nsecs); // 입력받은 nsecs 뒤에 SIGALRM 발생

	pause(); // 시그널 올때까지 sleep

	return alarm(0); // 설정된 모든 SIGALRM 해제 후 0 리턴
}

int	main()
{
	printf("Wait for 5 seconds...\n");

	mysleep(5); // 5초동안 sleep 하는 함수 실행
}
