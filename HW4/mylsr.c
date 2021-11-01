#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#define	MAX_PATH	256

void
JustDoIt(char *path)
{
	DIR				*dp;
	struct dirent	*dep;
	struct stat		statbuf;
	char			fullpath[MAX_PATH];

	if ((dp = opendir(path)) == NULL)  {
		perror("opendir");
		exit(0);
	}

	printf("\n%s:\n", path);
	while (dep = readdir(dp))  {
		if (strcmp(".", dep->d_name) == 0 || strcmp("..", dep->d_name) == 0)
			continue; // 자신, 자신 상위 디렉토리는 제외
		printf("%s\n", dep->d_name);
	}

	rewinddir(dp); // dp DIR 스트림을 처음으로 돌림. 첫 레코드를 가르킴.
	while (dep = readdir(dp))  {
		if (strcmp(".", dep->d_name) == 0 || strcmp("..", dep->d_name) == 0)
			continue;
		strcpy(fullpath, path);
		strcat(fullpath, "/");
		strcat(fullpath, dep->d_name);
		if (lstat(fullpath, &statbuf))  { // 해당되는 레코드의 stat 불러와 statbuf에 저장.
			perror("lstat");
			exit(1);
		}
		if (S_ISDIR(statbuf.st_mode))  { // 만약 레코드가 디렉토리이면, 재귀로 함수를 호출
			JustDoIt(fullpath);
		}
	}
	
	closedir(dp);
}

int main()
{
	JustDoIt(".");
}
