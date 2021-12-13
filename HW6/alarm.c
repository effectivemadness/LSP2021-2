#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

static unsigned int	AlarmSecs;


void
SigAlarmHandler(int signo) // SIG_ALRM 시그널 핸들링 함수
{
	if (signal(SIGALRM, SigAlarmHandler) == SIG_ERR)  { // SIG_ALRM에 핸들링 함수 등록
		perror("signal");
		exit(1);
	}

	alarm(AlarmSecs); // AlarmSecs 뒤에 SIG_ALRM 시그널 오게 등록

	/* Do something */
	printf("."); // 일정시간마다 실행되는 코드들
	fflush(stdout);

	return;
}

int
SetPeriodicAlarm(unsigned int nsecs)
{
	if (signal(SIGALRM, SigAlarmHandler) == SIG_ERR)  { // SIG_ALRM 에 핸들링 함수 등록
		return -1;
	}

	AlarmSecs = nsecs; // 전역 변수 AlarmSecs에 일정 시간 등록

	alarm(AlarmSecs); // AlarmSecs 뒤에 SIG_ALRM 시그널 오게 등록

	return 0;
}

int	main()
{
	printf("Doing something every one seconds\n");

	SetPeriodicAlarm(1); // 1초마다 특정 작동하는 함수 실행

	for ( ; ; )
		pause(); // 무한루프 돌며 sleep 하기 -> 일정시간마다 시그널 핸들링 뒤 또 sleep 반복 
}
