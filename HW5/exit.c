#include <stdio.h>
#include <stdlib.h>

void
myexit1()
{
	printf("first exit handler\n");
}

void
myexit2()
{
	printf("second exit handler\n");
}

int	main()
{
	/* Add myexit2 to exit functions */
	if (atexit(myexit2) != 0)  { // exit시에 실행하는 함수 등록
		perror("atexit");
		exit(1);
	}

	if (atexit(myexit1) != 0)  { //myexit1은 두번 등록됨. 실제 exit시에는 stack 과 같이 실행-> LIFO
		perror("atexit");
		exit(1);
	}

	if (atexit(myexit1) != 0)  {
		perror("atexit");
		exit(1);
	}

	printf("main is done\n");
}
