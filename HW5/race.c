#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>

void
CharAtaTime(char *str)
{
	char	*ptr;
	int		c, i;

	setbuf(stdout, NULL);
	for (ptr = str ; c = *ptr++ ; )  {
		for(i = 0 ; i < 999999 ; i++)
			; // 루프 발생 -> context sw 유도
		putc(c, stdout); //스트링의 한 글자씩 출력
	}
}

int	main()
{
	pid_t	pid;

	if ((pid = fork()) < 0)  { //fork 한 뒤
		perror("fork");
		exit(1);
	}
	// 서로 다른 스트링을 출력 -> race condidttion : stdout자원을 두고 경쟁
	else if (pid == 0)  {
		CharAtaTime("output from child\n");
	} 
	else  {
		CharAtaTime("output from parent\n");
	}
}
