#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
int
main()
{
	struct stat	statbuf;

	if (stat("bar", &statbuf) < 0)  {
		perror("stat");
		exit(1);
	}

	if (chmod("bar", (statbuf.st_mode & ~S_IWGRP) | S_ISUID) < 0)  { // bar 파일을 실행할 때, bar 파일의 소유자 권한으로 실행하게 됨.
		// 그룹 쓰기 권한 빼기.
		// root가 소유하고 있으면, 그 파일은 root 권한으로 실행됨.
		perror("chmod");
		exit(1);
	}

	/* S_IRUSR, S_IWUSR, S_IRGRP, S_IROTH : ON */
	if (chmod("foo", S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH) < 0)  {
		//foo 파일을 유저 읽기, 유저 쓰기, 그룹 읽기, others 읽기로 변경
		perror("chmod");
		exit(1);
	}
}
