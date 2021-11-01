#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <fcntl.h>
int main()
{
	umask(0);
	if (creat("bar", S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | 
				S_IROTH | S_IWOTH) < 0)  {
		perror("creat");
		exit(1);
	}

	/* Set file mode creation mask: S_IRGRP, S_IWGRP, S_IROTH, S_IWOTH */
	/* umask */
	umask(S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH); // umask를 그룹 쓰기/읽기, other 쓰기/읽기 로 설정
	if (creat("foo", S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | 
				S_IROTH | S_IWOTH) < 0)  { // 생성할때 받은 권한에서 umask를 제외하는 권한으로 파일 생성됨.
		perror("creat");
		exit(1);
	}
}
