#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>

int	main()
{
	pid_t	pid;
	int		status;

	if ((pid = fork()) < 0)  {
		perror("fork");
		exit(1);
	}
	else if (pid == 0)  {
		printf("I'm a child\n");
		sleep(2);
	}
	else  {
		/* wait */
		wait(&status); // 자식 프로세스가 종료될때까지 대기.
		// 종료된 자식 프로세스의 종료 상태를 status에 저장.
		printf("A child killed with %d status\n", status);
	}
}
