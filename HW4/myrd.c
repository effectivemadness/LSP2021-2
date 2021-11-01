#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
	if (argc != 2)  {
		fprintf(stderr, "Usage: %s dirname\n", argv[0]);
		exit(1);
	}

	if (rmdir(argv[1]) < 0)  {
		// argv[1] 로 받은 디렉토리를 삭제, 리턴값이 0이면 정상 삭제.
		// 에러 발생 시 -1 리턴되므로 if문 진입, 에러 메세지 출력
		perror("rmdir");
		exit(1);
	}
}
