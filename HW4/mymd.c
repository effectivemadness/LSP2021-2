#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
	if (argc != 2)  {
		fprintf(stderr, "Usage: %s dirname\n", argv[0]);
		exit(1);
	}

	/* 755 permission */
	umask(022); // group, others write 권한 제외하는 umask 설정
	if (mkdir(argv[1], 0777))  { // 입력받는 디렉토리 이름으로 새 디렉토리 생성
		//777로 생성해도 umask 제외되므로 755로 생성됨
		perror("mkdir");
		exit(1);
	}
}
