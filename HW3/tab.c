#include <stdio.h>
#include <string.h>
#include <stdlib.h>

enum { FALSE, TRUE };

void
conv(char *fname)
{
	FILE	*src, *dst;
	char	fdest[40];
	int		ch, first;

	if ((src = fopen(fname, "rt")) == NULL)  { // 읽을 파일 열기
		perror("fopen");
		return;
	}

	strcpy(fdest, fname);
	strcat(fdest, ".t");
	if ((dst = fopen(fdest, "wt")) == NULL)  { // 작성할 파일 열기
		perror("fopen");
		return;
	}

	first = TRUE; // 첫 글자 true로 설정
	while ((ch = fgetc(src)) != EOF)  { // 읽을 파일에서 한 글자씩 읽기
		if (first && ch == '\t')  { // 첫 글자이면서, 탭이면 
			fputc(' ', dst); // 스페이스 4개 작성
			fputc(' ', dst);
			fputc(' ', dst);
			fputc(' ', dst);
		}
		else  {
			fputc(ch, dst); //나머지 글자는 그대로 작성
			if (first) // 첫 글자이면, 다음 글자는 first = false
				first = FALSE;
			if (ch == '\n') // 줄이 끝나면, first = true
				first = TRUE;
		}
	}

	fclose(src);
	fclose(dst);
}

int
main(int argc, char *argv[])
{
	while (--argc)  {
		conv(argv[argc]); // argv[] 를 순회하면서 실행. 
	}
}
