#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
/*===============================================================
[Program Name] : uppper
[Description]  :
    - 지정된 파일을 읽어서 대문자로 변환하여 터미널에 출력한다
[Input]        :
    argv[2] : 입력받을 텍스트 파일의 이름
[Output]       :
    - 변수 및 리턴값으로 출력되는것은 없음
    - 터미널 stdout으로 대문자로 변환된 내용 출력
[Calls]        :
    - 없음(라이브러리)
[특기사항]     : 터미널 표준 출력으로 결과가 출력됨.
==================================================================*/
int main(int argc, char *argv[])
{
	FILE	*src; //파일 포인터 선언
	int	ch; // 입출력 할 문자 저장 변수 선언

	if (argc != 2)  { // 입력될 파일을 받지 못한경우
		fprintf(stderr, "Usage: %s source\n", argv[0]); //에러 메세지 출력
		exit(1); //종료
	}

	if ((src = fopen(argv[1], "rt")) == NULL)  { // 파일 열기 시도
		perror("fopen"); // 실패시 에러 메세지 출력
		exit(1); //종료
	}
	while ((ch = fgetc(src)) != EOF)  { // 파일의 끝까지(EOF) 한글자씩 읽기
		//src 파일 포인터가 가르키는 파일에서 한 글자 읽어 ch에 저장 (파일의 끝 만날때까지)
		if ('a' <= ch && ch <= 'z') //만약 읽은 문자가 소문자일 경우
			ch -= ('a' - 'A'); // 대문자로 변환
		write(1, &ch, 1); // stdout으로 출력 : 터미널에 출력
	}

	fclose(src); // 파일 닫기
}
