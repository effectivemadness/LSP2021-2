#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>

void
CharAtaTime(char *str)
{
	char	*ptr;
	int		c, i;

	setbuf(stdout, NULL);
	for (ptr = str ; c = *ptr++ ; )  {
		for(i = 0 ; i < 999999 ; i++)
			; // 무한루프 -> context sw 유도
		putc(c, stdout); // 한 글자씩 출력
	}
}

int	main()
{
	pid_t	pid;


	if ((pid = fork()) < 0)  {
		perror("fork");
		exit(1);
	}
	else if (pid == 0)  {
		CharAtaTime("output from child\n");
	} 
	else  {
		wait(NULL); // 자식이 모두 출력하고 종료될때까지 대기 후 수행
		CharAtaTime("output from parent\n");
	}
}
