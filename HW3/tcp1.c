#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
	FILE	*src, *dst;
	int		ch;

	if (argc != 3)  {
		fprintf(stderr, "Usage: %s source destination\n", argv[0]);
		exit(1);
	}

	if ((src = fopen(argv[1], "rt")) == NULL)  {
		perror("fopen");
		exit(1);
	}

	if ((dst = fopen(argv[2], "wt")) == NULL)  {
		perror("fopen");
		exit(1);
	}
	while ((ch = fgetc(src)) != EOF)  {
		//src 파일 포인터가 가르키는 파일에서 한 글자 읽어 ch에 저장 (파일의 끝 만날때까지)
		fputc(ch, dst); // dst 파일 포인터가 가르키는 파일에 읽은 한 글자 작성
	}

	fclose(src);
	fclose(dst);
}
