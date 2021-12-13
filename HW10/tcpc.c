#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "tcp.h"
#include <stdlib.h>
#include <unistd.h>
#include <strings.h>

int	main(int argc, char *argv[])
{
	int					sockfd, n; // 소켓의 fd, 읽은 길이 n 저장 변수
	struct sockaddr_in	servAddr; // 소켓 주소 저장할 구조체
	MsgType				msg; // 메세지 구조체타입

	if ((sockfd = socket(PF_INET, SOCK_STREAM, 0)) < 0)  { // PF_INET(ipv4), SOCK_STREAM(tcp) 프로토콜 소켓 생성
		perror("socket");
		exit(1);
	}

	bzero((char *)&servAddr, sizeof(servAddr)); // servAddr를 0으로 초기화
	servAddr.sin_family = PF_INET; // 주소체계 : IPV4
	servAddr.sin_addr.s_addr = inet_addr(SERV_HOST_ADDR); // 주소 넣어주기
	servAddr.sin_port = htons(SERV_TCP_PORT); // TCP 포트 빅엔디안으로 바꿔 넣기

	if (connect(sockfd, (struct sockaddr*)&servAddr, sizeof(servAddr)) < 0)  { // 정해진 주소, 포트 바탕으로 sockfd 소켓 연결
		perror("connect");
		exit(1);
	}

	msg.type = MSG_REQUEST; // 메세지 넣어주기
	sprintf(msg.data, "This is a request from %d.", getpid());
	if (write(sockfd, &msg, sizeof(msg)) < 0)  { // sockfd로 연결된 소켓에 메세지 작성
		perror("write");
		exit(1);
	}
	printf("Sent a request.....");
		
	if ((n = read(sockfd, &msg, sizeof(msg))) < 0)  { // 소켓으로 온 메세지 읽기
		perror("read");
		exit(1);
	}
	printf("Received reply: %s\n", msg.data); // 출력

	close(sockfd); // 소켓 닫기
}
