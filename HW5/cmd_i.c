#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define	MAX_CMD		256

void
DoCmd(char *cmd)
{
	printf("Doing %s", cmd); // 받은 명령 실행
	sleep(1); // 1초 sleep
	printf("Done\n"); // 종료!
}

int	main()
{
	char	cmd[MAX_CMD];

	while (1)  {
		printf("CMD> ");
		fgets(cmd, MAX_CMD, stdin);	// 입력받기
		if (cmd[0] == 'q') // 종료 명령이 아니면 
			break;

		DoCmd(cmd); //명령 실행
	}
}
