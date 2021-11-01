#include <stdio.h>
#include <stdlib.h>

#define	MAX_BUF	256

int main(int argc, char *argv[])
{
	FILE	*fp;
	char	buf[MAX_BUF];
	int		line;

	if (argc != 2)  {
		printf("Usage: %s filename\n", argv[0]);
		exit(1);
	}

	if ((fp = fopen(argv[1], "rt")) == NULL)  {
		perror("fopen");
		exit(1);
	}

	line = 1;
	while (fgets(buf, MAX_BUF, fp))  {
		// MAX_BUF개나, 중간에 개행문자 있을때까지 fp에서 읽어 buf에 저장.
		printf("%4d: %s", line++, buf); // buf 내용과, 입력받은 buf를 출력
	}

	fclose(fp);
}
