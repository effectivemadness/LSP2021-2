#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

#define	MAX_CMD		256

void
DoCmd(char *cmd)
{
	printf("Doing %s", cmd);
	sleep(1);
	printf("Done\n");
}

int	main()
{
	char	cmd[MAX_CMD];
	int		pid;

	while (1)  {
		printf("CMD> ");
		fgets(cmd, MAX_CMD, stdin);	 // 명령어 입력
		if (cmd[0] == 'q') //종료 명령어
			break; // loop 탈출 후 종료

		if ((pid = fork()) < 0)  { // fork 해서 자식 프로세스 생성
			perror("fork");
			exit(1);
		}
		else if (pid == 0)  { // 자식 프로세스에서는
			DoCmd(cmd); // 명령어 실행
			exit(0); // 명령어 실행 후 종료
		}
#if 1
		else  {
			wait(NULL); // 자식 프로세스가 종료하는것 대기 -> CMD> 출력되지 않고 자식 종료될때까지 대기.
		}
#endif
	}
}
