#include <stdio.h>
#include <signal.h>
#include <pwd.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

void
MyAlarmHandler(int signo)
{
	struct passwd	*rootptr; // passwd 구조체 사용할 포인터 변수 선언

	signal(SIGALRM, MyAlarmHandler); // SIG_ALRM에 시그널 핸들링 함수 등록
	alarm(1); // 1초 뒤에 SIG_ALRM 시그널 오게 설정

	printf("in signal handler\n");

	if ((rootptr = getpwnam("root")) == NULL)  { // root 이름으로 passwd 구조체 값 가져오기
		perror("getpwnam"); // 실패시 에러 출력
		exit(1);
	}

	return;
}

int	main()
{
	struct passwd	*ptr; // passwd 구조체 사용할 포인터 변수 선언

	signal(SIGALRM, MyAlarmHandler); // SIG_ALRM에 시그널 핸들링 함수 등록
	alarm(1); // 1초 뒤에 SIG_ALRM 시그널 오게 설정

	for ( ; ; )  { 
		if ((ptr = getpwnam("cjs")) == NULL)  { // cjs 이름으로 passwd 구조체 값 가져오기
			perror("getpwnam"); // 실패 시 에러 출력
			exit(1);
		}
		for (int i = 0; i < 99999999; i++) // 인위적으로 시그널 발생 대기 
				;
		if (strcmp(ptr->pw_name, "cjs") != 0)  { // ptr이 가르키는 구조체의 pw_name 이 "cjs"이 아닐 경우
			printf("return value corrupted!, pw_name = %s\n", ptr->pw_name); // 분명 "cjs"로 검색했는데, 값 이상 출력.
			exit(0);
		}
	}
}
