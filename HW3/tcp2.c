#include <stdio.h>
#include <stdlib.h>

#define	MAX_BUF	256

int main(int argc, char *argv[])
{
	FILE	*src, *dst;
	char	buf[MAX_BUF];

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

	while (fgets(buf, MAX_BUF, src))  {
		// src 파일의 끝을 만날때까지, MAX_BUF만큼이나 개행문자 만날때까지 buf에 저장.
		fputs(buf, dst); //저장된 buf를 dst 파일포인터가 가르키는 파일에 저장.
	}

	fclose(src);
	fclose(dst);
}
