#include <stdio.h>
#include <stdlib.h>

#define	MAX_BUF	1024

int main(int argc, char *argv[])
{
	FILE	*src, *dst;
	char	buf[MAX_BUF];
	int		count;

	if (argc != 3)  {
		fprintf(stderr, "Usage: %s source destination\n", argv[0]);
		exit(1);
	}

	if ((src = fopen(argv[1], "rb")) == NULL)  {
		perror("fopen");
		exit(1);
	}

	if ((dst = fopen(argv[2], "wb")) == NULL)  {
		perror("fopen");
		exit(1);
	}

	while ((count = fread(buf, 1, MAX_BUF, src)))  {
		// 1바이트 크기의 아이템을 MAX_BUF 만큼 src에서 읽어서 buf에 저장. 읽기 성공한 item의 개수를 count에 저장.
		fwrite(buf, 1, count, dst); // buf에 저장된 읽기 성공한 item의 개수만큼, 1바이트씩 dst에 저장.
	}

	fclose(src);
	fclose(dst);
}
