#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
int	main()
{
	pid_t	pid;
	char	*argv[] = { "mycp", "exec.c", "exec2.c", NULL };

	// execl
	if ((pid = fork()) < 0)  { // fork하여 
		perror("fork");
		exit(1);
	}
	else if (pid == 0)  {
		// mycp exec.c exec2.c
		if (execl("/home/lsp30/HW/HW5/mycp","mycp", "exec.c", "exec2.c", NULL) < 0)  { // arg list 형식으로 실행
			perror("execl");
			exit(1);
		}
	}
	if (waitpid(pid, NULL, 0) < 0)  { // fork 한 자식 프로세스 대기
		perror("waitpid");
		exit(1);
	}

	// execv
	if ((pid = fork()) < 0)  { // fork 하여
		perror("fork");
		exit(1);
	}
	else if (pid == 0)  {
		// mycp exec.c exec2.c
		if (execv("/home/lsp30/HW/HW5/mycp", argv) < 0)  { // argv 배열 형식으로 실행
			perror("execv");
			exit(1);
		}
	}
	if (waitpid(pid, NULL, 0) < 0)  { // fork 한 자식 프로세스 대기
		perror("waitpid");
		exit(1);
	}

	// execlp
	if ((pid = fork()) < 0)  { // fork 하여
		perror("fork");
		exit(1);
	}
	else if (pid == 0)  {
		// execute 'env' from PATH
		if (execlp("env", "env", NULL) < 0)  { // PATH에서 env 커맨드를 실행.
			perror("execlp");
			exit(1);
		}
	}
	if (waitpid(pid, NULL, 0) < 0)  {
		perror("waitpid");
		exit(1);
	}
}
