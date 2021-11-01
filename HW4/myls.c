#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdlib.h>

int main()
{
	DIR				*dp;
	struct dirent	*dep;

	if ((dp = opendir(".")) == NULL)  {
		perror("opendir");
		exit(0);
	}

	while (dep = readdir(dp))  {
		// DIR 를 입력으로 받아, 해당 디렉토리의 내용을 하나 읽어 dirent 구조체로 리턴.
		// 더이상 내용이 없을 경우 NULL 리턴 -> while 탈출
		printf("%s\n", dep->d_name); // 한 레코드의 이름 출력
	}
	
	closedir(dp);
}
