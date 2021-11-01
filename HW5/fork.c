#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>

int		Var = 6;
char	Buf[] = "a write to stdout\n";

int	main()
{
	int		var;
	pid_t	pid;

	var = 88;
	write(STDOUT_FILENO, Buf, sizeof(Buf)-1);
	printf("Before fork\n");

	if ((pid = fork()) < 0)  {
		perror("fork");
		exit(1);
	}
	else if (pid == 0)  {
		/* child */
		Var++;
		var++;
	}
	else  {
		/* parent */
		sleep(2);
	}

	printf("pid = %d, Var = %d, var = %d\n", getpid(), Var, var);
	// fork 한 뒤 값의 변경은 자식과 부모 각각 별도의 변수를 사용하게 됨.
}
