#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#define	MAX_BUF	1024

long
filesize(FILE *fp)
{
	long	cur, size;

	cur = ftell(fp);
	fseek(fp, 0L, SEEK_END);
	size = ftell(fp);
	fseek(fp, cur, SEEK_SET);
	return(size);
}

int main(int argc, char *argv[])
{
	FILE	*src, *dst1, *dst2;
	char	buf[MAX_BUF];
	int		count, size;

	if (argc != 4)  {
		fprintf(stderr, "Usage: %s source dest1 dest2\n", argv[0]);
		exit(1);
	}

	if ((src = fopen(argv[1], "rb")) == NULL)  {
		perror("fopen");
		exit(1);
	}
	if ((dst1 = fopen(argv[2], "wb")) == NULL)  {
		perror("fopen");
		exit(1);
	}
	if ((dst2 = fopen(argv[3], "wb")) == NULL)  {
		perror("fopen");
		exit(1);
	}

	size = filesize(src) / 2;

	while (size > 0)  {
		count = (size > MAX_BUF) ?  MAX_BUF : size; // size가 MAX_BUF보다 크면 MAX_BUF만큼, 작으면 size만큼 읽고쓰기
		fread(buf, 1, count, src); // 1바이트 아이템을 count 만큼 src에서 읽기.
		fwrite(buf, 1, count, dst1); // buf에 저장된 1 바이트 아이템을 count만큼 dst1이 가르키는 파일에 저장.
		size -= count;
	}

	while ((count = fread(buf, 1, MAX_BUF, src)) > 0)  {
		fwrite(buf, 1, count, dst2); // buf에 저장된 1 바이트 아이템을 count 만큼 dst2가 가르키는 파일에 저장
	}

	fclose(src);
	fclose(dst1);
	fclose(dst2);
}
