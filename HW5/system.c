#include <stdio.h>
#include <stdlib.h>

int	main()
{
	int		status;
	// 커맨드 실행 -> 정상 종료 - 0;
	if ((status = system("date")) < 0)  {
		perror("system");
		exit(1);
	}
	printf("exit status = %d\n", status);
	
	// 수행할 수 없는 명령어에서 종료 -> 127(command not found) * 256 결과 나옴.
	if ((status = system("nosuchcommand")) < 0)  {
		perror("system");
		exit(1);
	}
	printf("exit status = %d\n", status);

	/* $ who; exit 44 */
	// exit 44 -> 44 * 256 결과 나옴
	if ((status = system("who; exit 44")) < 0)  {
		perror("system");
		exit(1);
	}
	printf("exit status = %d\n", status);
}
