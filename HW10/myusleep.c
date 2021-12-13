#include <stdio.h>
#include <sys/types.h>
#include <sys/time.h>
#include <unistd.h>

void
myusleep(int secs, int usecs) // select 함수로 제작한 usleep 함수
{
	struct timeval	tm;

	tm.tv_sec = secs; // 초
	tm.tv_usec = usecs; // 마이크로초

	if (select(0, (fd_set*)0, (fd_set*)0, (fd_set*)0, &tm) < 0)  { // 읽을 데이터 있는 지 감시, 쓸 수 있는지 감시, 예외사항 감시 모두 없고 단순히 timeout만 설정한 select
	// timeout 지났을때 종료 됨
		perror("select");
		return;
	}
}

int	main()
{
	printf("Sleep for 1.5 seconds.....\n");
	myusleep(1, 500000);
}
