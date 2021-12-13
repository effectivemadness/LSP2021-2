#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/type.h>

int 
main() 
{
	int fd[2]; // 부모 -> 자식 파이프
	pid_t child;

	if(pipe(fd) == -1)  { // 파이프 생성
		perror("pipe"); // 에러 발생 처리
		exit(1);
	}
 
	child = fork(); // 자식 프로세스 생성
	if(child == (pid_t)(-1))  {
        	perror("fork"); // fork 실패 에러처리
		exit(1);
	}
	else if(child == (pid_t)0) { // 자식 프로세스일 경우
        	close(1); //stdout 닫기
		close(fd[0]); // 안쓰는 한쪽 pipe end 닫기
        	
        	if(dup(fd[1]) == -1)  { // fd[1]과 연결할 수 있는 가장 낮은 fd 묶기 - stdout
        		perror("dup");
			exit(1);
		}

//			for (int i = 0 ; i < 100000000; i++) ; // 자식에서 오래 대기해 파이프에 작성 못하고 context sw 상황 만들어보기
        	if((execlp("ls", "ls", "-l", NULL)) == -1)  { // ls -l 실행 -> stdout 값이 fd[1]으로 보내짐
        		perror("execlp");
			exit(1);
		}
	} 
	else { // 부모일 경우
        	close(0); // stdin 닫기
		close(fd[1]); // 안쓰는 한쪽 pipe end 닫기
        	
        	if(dup2(fd[0], 0) == -1)  { // fd[0]을 stdin과 묶기
        		perror("dup"); // 에러 처리
			exit(1);
		}

        	if((execlp("more", "more", NULL)) == -1)  { // more 실행 -> fd[0]에서 읽어서 실행
        		perror("execlp");
			exit(1);
		}
	}
 
	return 0;
}
