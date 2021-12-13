#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include "tcp.h"
#include <unistd.h>
#include <stdlib.h>
#include <strings.h>

int	Sockfd;

void
CloseServer() // 서버 종료 함수
{
	close(Sockfd); // 소켓 파일디스크립터 닫기
	printf("\nTCP Server exit.....\n");

	exit(0);
}


int	main(int argc, char *argv[])
{
	int					newSockfd, cliAddrLen, n;
	struct sockaddr_in	cliAddr, servAddr;
	MsgType				msg;

	signal(SIGINT, CloseServer);

	if ((Sockfd = socket(PF_INET, SOCK_STREAM, 0)) < 0)  { // 소켓 열기
		perror("socket");
		exit(1);
	}

	bzero((char *)&servAddr, sizeof(servAddr)); // 0으로 초기화
	servAddr.sin_family = PF_INET; // 인터넷 프로토콜
	servAddr.sin_addr.s_addr = htonl(INADDR_ANY); // 모든 주소에서 오는 데이터 허용, 비트 순서 빅엔디안으로 바꾼 뒤 저장
	servAddr.sin_port = htons(SERV_TCP_PORT); // 포트 비트 순서 빅엔디안으로 바꾼 뒤 저장

	if (bind(Sockfd, (struct sockaddr *) &servAddr, sizeof(servAddr)) < 0)  { // 포트에 TCP 소켓 바인드
		perror("bind");
		exit(1);
	}

	listen(Sockfd, 5); // Socketfd 소켓 listen, 최대 5개까지의 연결 요청 대기 가능함

	printf("TCP Server started.....\n");

	cliAddrLen = sizeof(cliAddr); // 클라이언트 어드레스 길이
	while (1)  {
		newSockfd = accept(Sockfd, (struct sockaddr *) &cliAddr, &cliAddrLen); //소켓에서 연결 요청된것 받아서 새 fd로 받아오기
		if (newSockfd < 0)  {
			perror("accept"); // 에러처리
			exit(1);
		}
		
		if ((n = read(newSockfd, (char *)&msg, sizeof(msg))) < 0)  { // 해당 소켓에서 msg만큼 읽기
			perror("read");
			exit(1);
		}
		printf("Received request: %s.....", msg.data); // msg 데이터 값 출력

		msg.type = MSG_REPLY; // 보낼 메세지 타입 설정
		sprintf(msg.data, "This is a reply from %d.", getpid()); // msg에 데이터 넣어주기
		if (write(newSockfd, (char *)&msg, sizeof(msg)) < 0)  { // accept된 소켓으로 reply 보내기
			perror("write");
			exit(1);
		}
		printf("Replied.\n");

		usleep(10000); // 잠시 대기
		close(newSockfd); // 소켓 닫기
	}
}
