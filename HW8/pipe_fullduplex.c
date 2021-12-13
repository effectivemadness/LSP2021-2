#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#define	MAX_BUF	128
/*===============================================================
[Program Name] : pipe_fullduplex
[Description]  : 
    - parent process와 child process간의 pipe를 사용해 메세지를 주고 받는다
[Input]        :
    -
[Output]       :
	- shell 출력으로 프로세스간 통신 결과를 출력
[Calls]        :	       
	-
[특기사항]     : pipe.c 실습 코드에 파이프 추가하여 구현
==================================================================*/
int	main()
{
	int		n, fd1[2], fd2[2]; // 바이트 수, 파이프 file descriptor 저장 변수 선언
	// c fd1[0] <- fd1[1] p
	// p fd2[0] <- fd2[1] c
	pid_t	pid; // pid 저장
	char	buf[MAX_BUF]; // 임시로 사용할 버퍼 선언

	if (pipe(fd1))  { // pipe 생성
		perror("pipe"); // 에러발생 처리
		exit(1);
	}
	if (pipe(fd2)) { // pipe 생성
		perror("pipe"); // 에러발생 처리
		exit(1);
	}

	if ((pid = fork()) < 0)  { // fork 실행
		perror("fork"); // 에러발생 처리
		exit(1);
	}
	else if (pid == 0)  { // 자식 프로세스 일 때
		close(fd1[1]); // fd1[1]은 사용하지 않으므로 닫기
		close(fd2[0]); // fd2[0]은 사용하지 않으므로 닫기
		printf("Child : Wait for parent to send data\n");
		if ((n = read(fd1[0], buf, MAX_BUF)) < 0)  { // fd[0]에서 MAX_BUF만큼 읽어오기
			perror("read");
			exit(1);
		}
		printf("Child : Received data from parent: ");
		fflush(stdout);
		write(STDOUT_FILENO, buf, n); // 읽은 버퍼만큼 출력.
		strcpy(buf, "Hello, World! from child\n");
		printf("Child: Send data to parent\n"); 
		write(fd2[1], buf, strlen(buf));
	}
	else  { // 부모프로세스 일때
		close(fd1[0]); // fd1[0]은 사용하지 않으므로 닫기
		close(fd2[1]); // fd2[1]은 사용하지 않으므로 닫기
		strcpy(buf, "Hello, World! from parent\n"); // buf에 스트링 넣기
		printf("Parent: Send data to child\n"); 
		write(fd1[1], buf, strlen(buf)); // 파이프에 buf 작성
		if ((n = read(fd2[0], buf, MAX_BUF)) < 0)  { // fd2[0]에서 MAX_BUF만큼 읽어오기
			perror("read");
			exit(1);
		}
		printf("Parent : Received data from child: ");
		fflush(stdout);
		write(STDOUT_FILENO, buf, n); // 읽은 버퍼만큼 출력.
	}

	exit(0);
}
