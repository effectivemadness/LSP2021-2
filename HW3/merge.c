#include <stdio.h>
#include <stdlib.h>
#define	MAX_BUF	1024

int main(int argc, char *argv[])
{
	FILE	*src1, *src2, *dst;
	char	buf[MAX_BUF];
	int		count;

	if (argc != 4)  {
		fprintf(stderr, "Usage: %s source1 source2 dest\n", argv[0]);
		exit(1);
	}

	if ((src1 = fopen(argv[1], "rb")) == NULL)  {
		perror("fopen");
		exit(1);
	}
	if ((src2 = fopen(argv[2], "rb")) == NULL)  {
		perror("fopen");
		exit(1);
	}
	if ((dst = fopen(argv[3], "wb")) == NULL)  {
		perror("fopen");
		exit(1);
	}

	while ((count = fread(buf, 1, MAX_BUF, src1)))  { // src1이 가르키는 파일에서 1바이트 아이템을 최대 MAX_BUF개 만큼 읽어 buf에 저장. 읽은 아이템 개수는 count에 저장.
		fwrite(buf, 1, count, dst);
	}

	while ((count = fread(buf, 1, MAX_BUF, src2)))  { // src2가 가르키는 파일에서 1바이트 아이템을 최대 MAX_BUF개 만큼 읽어 buf에 저장. 읽은 아이템 개수는 count에 저장.
		fwrite(buf, 1, count, dst);
	}

	fclose(src1);
	fclose(src2);
	fclose(dst);
}
