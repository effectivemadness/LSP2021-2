#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include "fifo.h"
#include <stdlib.h>

void
SigIntHandler(int signo) // 종료 시그널 오면 처리
{
	if (remove(SERV_FIFO) < 0)  { // 만들어둔 cli->serv fifo 삭제
		perror("remove");
		exit(1);
	}

	exit(0); // 종료
}


int	main()
{
	int			fd, cfd, n;
	MsgType		msg;

	if (signal(SIGINT, SigIntHandler) == SIG_ERR)  { // 종료시 시그널 처리 함수 등록
		perror("signal");
		exit(1);
	}

	if (mkfifo(SERV_FIFO, 0600) < 0)  { // cli->serv fifo 만들기
		if (errno != EEXIST)  { // 이미 있는건 정상 간주
			perror("mkfifo");
			exit(1);
		}
	}

	if ((fd = open(SERV_FIFO, O_RDWR)) < 0)  { // cli->serv fifo 열기
		perror("open");
		exit(1);
	}

	while (1)  {
		if ((n = read(fd, (char *)&msg, sizeof(msg))) < 0)  { // cli->serv fifo 에서 계속 읽기
			if (errno == EINTR)  {
				continue;
			}
			else  {
				perror("read");
				exit(1);
			}
		}
		printf("Received request: %s.....", msg.data); // 받은 요청 출력
		fflush(stdout);
		if ((cfd = open(msg.returnFifo, O_WRONLY)) < 0)  { // msg에 있는 serv->cli fifo 열기
			perror("open");
			exit(1);
		}
		sprintf(msg.data, "This is a reply from %d.", getpid()); // serv->cli 메세지 만들기
		write(cfd, (char *)&msg, sizeof(msg)); // serv->cli fifo에 작성
		close(cfd); // 닫기
		printf("Replied.\n");
	}
}
