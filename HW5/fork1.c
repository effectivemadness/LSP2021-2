#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>

int	main()
{
	int		pid;

	if ((pid = fork()) < 0)  {
		perror("fork");
		exit(1);
	}
	else if (pid == 0)  { // 자식의 경우 fork() 리턴값이 0
		/* child */
		printf("I am %d. My parent is %d.\n", getpid(), getppid());
	}
	else  { // 부모의 경우 fork() 리턴값이 자식 pid
		/* parent */
		printf("I am %d. My child is %d.\n", getpid(), pid);
	}
}
