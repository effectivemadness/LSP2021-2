#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
	if (argc != 3)  {
		fprintf(stderr, "Usage: %s old new\n", argv[0]);
		exit(1);
	}

	if (rename(argv[1], argv[2]))  {
		// argv[1] 의 파일을 argv[2]로 이동(및 이름 변경). 실패시 -1리턴하므로 if 조건에 걸려 에러 메세지 출력/
		perror("rename");
		exit(1);
	}
}
