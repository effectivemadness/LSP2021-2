#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#define	MAX_CMD		256

void
DoCmd(char *cmd)
{
	printf("Doing %s", cmd);
	sleep(1);
	printf("Done\n");

	pthread_exit(NULL);
}

int	main()
{
	char		cmd[MAX_CMD];
	pthread_t	tid;

	while (1)  {
		printf("CMD> ");
		fgets(cmd, MAX_CMD, stdin);	// 명령어 입력
		if (cmd[0] == 'q') // 종료 명령어인경우 
			break; // loop 탈출 후 종료

		if ((pthread_create(&tid, NULL, (void *)DoCmd, (void *)cmd)) < 0)  { // 쓰레드 생성 -> DoCmd 함수, 인자는 cmd. 정상적 생성되면 itd에 쓰레드 id 저장
			perror("pthread_create");
			exit(1);
		}

#if 1
		pthread_join(tid, NULL); // 쓰레드 종료 대기 후 실행
#endif
	}
}
