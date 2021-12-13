#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include "fifo.h"
#include <stdlib.h>
#include <string.h>

int	main()
{
	char		fname[MAX_FIFO_NAME]; // 파일이름 버퍼 만들기
	int			fd, sfd, n; // 사용할 변수들 선언
	MsgType		msg; // 주고받을 메세지 선언

	sprintf(fname, ".fifo%d", getpid()); // pid를 붙인 fifo 파일 이름 만들기
	if (mkfifo(fname, 0600) < 0)  { // fifo 생성 
		perror("mkfifo"); // 에러 처리
		exit(1);
	}

	if ((fd = open(fname, O_RDWR)) < 0)  { // cli->serv fifo 열기
		perror("open");
		exit(1);
	}
	if ((sfd = open(SERV_FIFO, O_RDWR)) < 0)  { // serv->cli fifo 열기
		perror("open");
		exit(1);
	}

	strcpy(msg.returnFifo, fname); // msg 구조체 returnFifo에 만든 파일이름 넣기
	sprintf(msg.data, "This is a request from %d.", getpid()); // msg 구조체에 데이터 넣기
	write(sfd, (char *)&msg, sizeof(msg)); // 서버에 요청 보내기
	printf("Sent a request.....");
	fflush(stdout);

	if ((n = read(fd, (char *)&msg, sizeof(msg))) < 0)  { // serv->cli fifo 에서 msg 읽어오기
		perror("read");
		exit(1);
	}

	printf("Received reply: %s\n", msg.data); // 받은 데이터 출력

	close(fd); // fifo들 닫기
	close(sfd);

	if (remove(fname) < 0)  { // 만든 fifo 파일 삭제하기
		perror("remove");
		exit(1);
	}
}
