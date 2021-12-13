#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <signal.h>

/*===============================================================
[Function Name] : void	SigChldHandler
[Description]   : 
    - SIGCHLD 시그널의 핸들링 함수로써, 자식 프로세스가 종료되었음을 알리고 종료한다. 
[Input]         :
    int	signo		// 시그널 번호
[Output]        :
	Nothing
[Call By]       :	        
    SIGCHLD signal
[Calls]         :	        
    Nothing
[Given]         :
    Nothing
[Returns]       :
    Nothing
==================================================================*/
void	SigChldHandler(int signo)
{
		printf("Received a SIGCHLD Signal\n"); // 자식 종료 시그널 받음 출력
		printf("Child process terminated\n");
		printf("Terminating this process\n");

		exit(0); // 프로세스 종료
}
/*===============================================================
[Program Name] : async_child
[Description]  : 
    - 자식 프로세스를 생성하고, 자식 프로세스가 종료되는 것을 시그널을 사용해 비동기 방식으로 처리한다.
[Input]        :
    - 
[Output]       :
    - 쉘 표준출력 : 자식 프로세스 실행 및 종료 메세지
[Calls]        :	        
    - SIGCHLD signal - SigChldHandler(int signo)
[특기사항]     : 시그널 핸들링 함수를 설정하고 자식 프로세스 생성 한 뒤 무한루프
==================================================================*/
int	main()
{
	pid_t	pid;

	if (signal(SIGCHLD, SigChldHandler) == SIG_ERR)	{ // SIGCHLD 시그널에 핸들링 함수 붙이기
		perror("signal"); // 실패시 에러 메세지 출력
		exit(1);
	}
	if ((pid = fork()) < 0)  { // fork 진행
		perror("fork");
		exit(1);
	}
	else if (pid == 0)  { // 자식 프로세스는 2초 뒤 종료
		printf("I'm a child\nTerminating in 2 secs\n");
		sleep(2);
	}
	else  {
		while (1); // 부모 프로세스는 다른 작업을 진행하다가(while loop) SIGCHLD 시그널이 오면 핸들러에서 종료.
	}
}
