#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include "udp.h"


int	Sockfd;

void
CloseServer() // 서버 종료 시그널 핸들러
{
	close(Sockfd); // socket 닫기
	printf("\nUDP Server exit.....\n");

	exit(0);
}


int	main(int argc, char *argv[])
{
	int					cliAddrLen, n; // 클라이언트 길이, 수신한 데이터 길이 저장 변수
	struct sockaddr_in	cliAddr, servAddr; // 클라이언트, 서버 주소 구조체
	MsgType				msg; // 메세지 구조체 타입

	signal(SIGINT, CloseServer); // 서버 종료 시그널 핸들러 붙이기

	if ((Sockfd = socket(PF_INET, SOCK_DGRAM,0)) < 0)  { // PF_INET : ipv4, SOCK_DGRAM : udp 프로토콜로 소켓 생성
		perror("socket");
		exit(1);
	}

	bzero((char *)&servAddr, sizeof(servAddr)); // servAddr 구조체 0으로 초기화
	servAddr.sin_family = PF_INET; // servAddr : ipv4 
	servAddr.sin_addr.s_addr = htonl(INADDR_ANY); // Address to accept any incoming messages. : 모든 메세지 허용 비트 순서 빅엔디안으로 바꾼 뒤 저장
	servAddr.sin_port = htons(SERV_UDP_PORT); // 포트 번호 비트 순서 빅엔디안으로 바꾼 뒤 저장

	if (bind(Sockfd, (struct sockaddr*)&servAddr, sizeof(servAddr)) < 0)  { // 소켓 파일 디스크립터와 주소 정보를 묶기
		perror("bind");
		exit(1);
	}

	printf("UDP Server started.....\n");

	cliAddrLen = sizeof(cliAddr); // cliAddr 구조체 길이 저장
	while (1)  {
		if ((n = recvfrom(Sockfd, (char*)&msg, sizeof(msg), 0, (struct sockaddr*)&cliAddr, &cliAddrLen)) < 0)  { // cliAddr에서 데이터 가져오기 : cliAddr에 포트와 주소가 저장되고, 받은 메세지 길이 리턴
			perror("recvfrom");
			exit(1);
		}
		printf("Received request: %s.....", msg.data);

		msg.type = MSG_REPLY; // msg의 타입을 답장으로 바꾸기
		sprintf(msg.data, "This is a reply from %d.", getpid()); // msg에 데이터 넣기
		if (sendto(Sockfd, (char*)&msg, sizeof(msg), 0, (struct sockaddr*)&cliAddr, cliAddrLen) < 0)  { // cliAddr로 데이터 보내기
			perror("sendto");
			exit(1);
		}
		printf("Replied.\n");
	}
}
