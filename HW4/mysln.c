#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
	if (argc != 3)  {
		fprintf(stderr, "Usage: %s source destinaion\n", argv[0]);
		exit(1);
	}

	if (symlink(argv[1], argv[2]))  {
		// argv[1] : src 파일에 대한 심볼릭 링크를 argv[2] : target에 생성
		perror("link");
		exit(1);
	}
}
