#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
	if (argc != 2)  {
		fprintf(stderr, "Usage: %s dirname\n", argv[0]);
		exit(1);
	}

	if (chdir(argv[1]))  { 
		// chdir로 cwd를 변경. -> 성공하면 0 리턴, 실패시 -1리턴하여 에러메시지 출력
		perror("chdir");
		exit(1);
	}
}
