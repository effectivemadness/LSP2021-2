#include <stdio.h>

int	main(int argc, char *argv[], char *envp[])
{
	int			i;
	char		**p;
	extern char	**environ; // 실행할때 자동으로 만들어주는 환경 변수를 담는 2차원 char 문자열배열
	// 마지막 값 이후는 null

	printf("List command-line arguments\n");
	for (i = 0 ; i < argc ; i++)  {
		printf("%s\n", argv[i]);
	}

	printf("\n");
	printf("List environment variables from environ variable\n");
#if 1
	for (i = 0 ; environ[i] != NULL ; i++)  { // environ 변수를 사용한 출력
		printf("%s\n", environ[i]); // 배열 notation 사용
	}
#else
	for (p = environ ; *p != NULL ; p++)  { // environ 변수를 사용한 출력
		printf("%s\n", *p); // pointer notation 사용.
	}
#endif

	printf("\n");
	printf("List environment variables from envp variable\n");
#if 1
	for (i = 0 ; envp[i] != NULL ; i++)  { // envp 변수를 사용한 출력
		printf("%s\n", envp[i]); // 배열 notation 사용
	}
#else
	for (p = envp ; *p != NULL ; p++)  { //envp 변수를 사용한 출력
		printf("%s\n", *p); // pointer notation 사용
	}
#endif
}
