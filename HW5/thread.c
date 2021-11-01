#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

void
PrintMsg(char *msg)
{
	printf("%s", msg);

	pthread_exit(NULL); // 쓰레드 종료 - 종료상태 NULL
}

int	main()
{
	pthread_t	tid1, tid2;
	char		*msg1 = "Hello, ";
	char		*msg2 = "World!\n";

	/* Thread ID: tid1, Thread function: PrintMsg, Thread argument: msg1 */
	if (pthread_create(&tid1, NULL, (void *)PrintMsg, (void *)msg1) < 0)  {
			//새로운 thread 생성 : threadid = tid1에 저장, PrintMsg 함수에 msg1 인자로 실행.
		perror("pthread_create");
		exit(1);
	} 

	if (pthread_create(&tid2, NULL, (void *)PrintMsg, (void *)msg2) < 0)  {
			//새로운 thread 생성 : threadid = tid2에 저장, PrintMsg 함수에 msg2 인자로 실행.
		perror("pthread_create");
		exit(1);
	}

	printf("Threads created: tid=%lu, %lu\n", tid1, tid2);
	
	/* Wait for tid1 to exit */
	if (pthread_join(tid1, NULL) < 0)  { //쓰레드가 종료할때까지 대기 - 종료상태는 저장 안함(NULL)
		perror("pthread_join");
		exit(1);
	}
	if (pthread_join(tid2, NULL) < 0)  { //쓰레드가 종료할때까지 대기 - 종료상태는 저장 안함(NULL)
		perror("pthread_join");
		exit(1);
	}

	printf("Threads terminated: tid=%lu, %lu\n", tid1, tid2);
}
