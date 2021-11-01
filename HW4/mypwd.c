#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#define	MAX_BUF	256

int main(int argc, char *argv[])
{
	char	buf[MAX_BUF];

	if (!getcwd(buf, MAX_BUF))  {
		// 현재 작업중인 디렉토리를 buf에 저장.
		// 가져오는것 실패시 NULL 리턴 -> 에러 메세지 출력.
		perror("getcwd");
		exit(1);
	}

	printf("%s\n", buf);
}
