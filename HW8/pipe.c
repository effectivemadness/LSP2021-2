#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#define	MAX_BUF	128

int	main()
{
	int		n, fd[2]; // 바이트 수, 파이프 file descriptor 저장 변수 선언
	pid_t	pid; // pid 저장
	char	buf[MAX_BUF]; // 임시로 사용할 버퍼 선언

	if (pipe(fd))  { // pipe 생성
		perror("pipe"); // 에러발생 처리
		exit(1);
	}

	if ((pid = fork()) < 0)  { // fork 실행
		perror("fork"); // 에러발생 처리
		exit(1);
	}
	else if (pid == 0)  { // 자식 프로세스 일 때
		close(fd[1]); // fd[1]은 사용하지 않으므로 닫기
		printf("Child : Wait for parent to send data\n");
		if ((n = read(fd[0], buf, MAX_BUF)) < 0)  { // fd[0]에서 MAX_BUF만큼 읽어오기
			perror("read");
			exit(1);
		}
		printf("Child : Received data from parent: ");
		fflush(stdout);
		write(STDOUT_FILENO, buf, n); // 읽은 버퍼만큼 출력.
	}
	else  { // 부모프로세스 일때
		close(fd[0]); // fd[0]은 사용하지 않으므로 닫기
		strcpy(buf, "Hello, World!\n"); // buf에 스트링 넣기
		printf("Parent: Send data to child\n"); 
		write(fd[1], buf, strlen(buf)); // 파이프에 buf 작성
	}

	exit(0);
}
