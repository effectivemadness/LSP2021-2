#include <unistd.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <stdlib.h>
/*===============================================================
[Function Name] : int mysystem(const char *cmd)
[Description]   : 
    - cmd 에서 세미콜론으로 각 명령어를 분리 후 공백 기준으로 명령어, arg 분리하여 실행
	- strtoc_r을 사용해 문자열에서 세미콜론과 공백 기준으로 분리
	- 분리한 명령어가 없는경우(command not found), exit인 경우 예외로 따로 처리 구현
[Input]         :
    const char *cmd;	// 실행 할 커맨드 문자열
[Output]        :
    int	mysystem()	// 커맨드 실행 결과
[Call By]       :	        
    Main()
[Calls]         :	        
    Nothiong
[Given]         :
    Nothing
[Returns]       :
    Nothing
==================================================================*/
int mysystem(const char *cmd)
{
	int pid; //fork 자식 pid 변수
	int cmd_cnt; // 공백으로 구분되는 명령어 개수
	int stat; // 반환될 실행 결과 상태 
	char *cmd_string = strdup(cmd); // strtok_r에서 사용하기 위해 사본 생성

	char *semi_sep; // ;으로 구분될 명령어 저장
	char *semi_sep_next; // 다음 명령어 위치
	semi_sep = strtok_r(cmd_string, ";", &semi_sep_next); // ;으로 명령어 나누기
	while (semi_sep != NULL) // ;으로 나눈 명령어 순회
	{
		cmd_cnt = 0; // 공백으로 나누어질 명령어 개수 초기화
		char *ptr;
		char *ptr_next;
		char *semi_sep_dup = strdup(semi_sep); // strtok_r 에서 사용하기 위해 사본 생성
		char *free_pos = semi_sep_dup; // free할 포인터 저장
		ptr = strtok_r(semi_sep, " ", &ptr_next); // 공백으로 명령어 나누기
		while (ptr != NULL)
		{
			ptr = strtok_r(NULL, " ", &ptr_next);
			cmd_cnt++; // 순회하면서 개수 세기
		}
		char **split_cmd;
		split_cmd = malloc(sizeof(char *) * (cmd_cnt + 1)); // arg 저장할 배열 생성
		ptr = strtok_r(semi_sep_dup, " ", &ptr_next); // 공백으로 다시 나누기 시작
		int i = 0;
		while (ptr != NULL)
		{
			split_cmd[i] = ptr; // 배열에 하나씩 저장
			ptr = strtok_r(NULL, " ", &ptr_next);
			i++;
		}
		split_cmd[i] = NULL; // 마지막 배열 자리는 NULL로 넣어주기
		if (strcmp(split_cmd[0], "exit") == 0) // 명령어가 exit일 경우 처리
		{
			if (split_cmd[1])
				stat = atoi(split_cmd[1]); // exit 코드 숫자로 바꾸기
			else
				stat = 0; // exit 뒤에 숫자 없을때 0으로 종료
			free(free_pos); // 복사 해둔 str free
			semi_sep = strtok_r(NULL, ";", &semi_sep_next); // ;으로 나뉜 다음 명령어 실행
			continue;
		}
		cmd_cnt = strlen(split_cmd[0]); // 명령어 길이 설정 
		cmd_cnt += strlen("which "); // "which " 더하기
		char *which_cmd = malloc(cmd_cnt + 1); // 새 문자열 공간 생성
		snprintf(which_cmd, cmd_cnt + 1, "which %s", split_cmd[0]); // which 실행할 스트링 생성
		FILE *fp = popen(which_cmd, "r"); // 파이프로 which 실행

		free(which_cmd); // 이제 사용 안하는 which_cmd free
		char abs_cmd[1024]; // 절대경로 스트링 버퍼
		if (!(fp && fgets(abs_cmd, sizeof(abs_cmd), fp))) // 파이프에서 읽어올게 없는 경우
		{
			stat = 127; // command not found 상태 설정
			semi_sep = strtok_r(NULL, ";", &semi_sep_next); // ; 으로 나뉜 다음 명령어 실행
			free(free_pos); // 복사해둔 str free
			continue;

		}
		pclose(fp); // 파이프 닫기
		abs_cmd[strlen(abs_cmd) - 1] = '\0'; // 마지막 개행문자 삭제
		split_cmd[0] = abs_cmd; // 명령어를 절대경로로 넣어주기
		
		if ((pid = fork()) < 0) // fork 실행
		{
			perror("fork"); // 에러발생시 출력
			exit(1);
		}
		else if (pid == 0) // 자식일 경우 execv 실행
		{
		if (execv(split_cmd[0], split_cmd) < 0)
			{
				free(free_pos); // 실행을 못했을 경우 전부 free 후 에러처리
				free(split_cmd);
				perror("execv");
				exit(1);
			}
		}
		else
		{
			if (waitpid(pid, &stat, 0) < 0) // 자식 process를 대기
			{
				perror("waitpid"); // waitpid 실패시 에러 출력
				exit(1);
			}
			free(free_pos); // 복사해푼 str free
			semi_sep = strtok_r(NULL, ";", &semi_sep_next); // ;으로 나뉜 다음 명령어 실행
		}
	}
	free(cmd_string); // 복사해둔 cmd free
	return (stat << 8); // system 리턴값은 stat*256 으로 리턴됨
}

/*===============================================================
[Program Name] : mysystem
[Description]  : 
    - system() 함수와 동일하게 작동하는 함수 mysystem()을 실행하여 입출력을 비교한다.
[Input]        :
    -
[Output]       :
    - 각 mysystem() 함수의 실행을 해보고, 리턴되는 값을 stdout으로 출력한다.
[Calls]        :	        
    int	mysystem(const char *cmd);
[특기사항]     : system 함수 실습 코드를 기반으로 작성.
==================================================================*/
int main()
{
	int status;

	if ((status = mysystem("date")) < 0)
	{
		perror("system");
		exit(1);
	}
	printf("exit status = %d\n", status);

	if ((status = mysystem("nosuchcommand")) < 0)
	{
		perror("system");
		exit(1);
	}
	printf("exit status = %d\n", status);

	if ((status = mysystem("who; exit 44")) < 0)
	{
		perror("system");
		exit(1);
	}
	printf("exit status = %d\n", status);
}