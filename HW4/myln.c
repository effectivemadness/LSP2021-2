#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
int
main(int argc, char *argv[])
{
	if (argc != 3)  {
		fprintf(stderr, "Usage: %s source destinaion\n", argv[0]);
		exit(1);
	}

	if (link(argv[1], argv[2]) < 0)  {
		// src 파일의 hardlink를 dest에 생성
		perror("link");
		exit(1);
	}
}
