#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "udp.h"


int	main(int argc, char *argv[])
{
	int					sockfd, n, peerAddrLen; // 소켓 파일 디스크립터, 상대방 주소 길이 저장 변수
	struct sockaddr_in	servAddr, peerAddr; // 서버 주소, 보낼 주소 저장할 구조체
	MsgType				msg; // 메세지 구조체 타입

	if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)  { // PF_INET : ipv4, SOCK_DGRAM : udp 프로토콜로 소켓 생성
		perror("socket");
		exit(1);
	}

	bzero((char *)&servAddr, sizeof(servAddr)); // servAddr 구조체 0으로 초기화
	servAddr.sin_family = PF_INET; // servAddr : ipv4 
	servAddr.sin_addr.s_addr = inet_addr(SERV_HOST_ADDR); // 서버 주소 를 빅엔디안으로 바꾼 뒤 저장
	servAddr.sin_port = htons(SERV_UDP_PORT); // 서버 포트를 빅엔디안으로 바꾼 뒤 저장

	msg.type = MSG_REQUEST; // 메세지 타입 : 요청
	sprintf(msg.data, "This is a request from %d.", getpid()); // 메세지 내용 작성
	if (sendto(sockfd, (char*)&msg, sizeof(msg), 0, (struct sockaddr*)&servAddr, sizeof(servAddr)) < 0)  { // 서버 주소로 msg 보내기
		perror("sendto");
		exit(1);
	}
	printf("Sent a request.....");
		
	peerAddrLen = sizeof(peerAddr); // peerAddr의 길이 저장
	if ((n = recvfrom(sockfd, (char*)&msg, sizeof(msg), 0, (struct sockaddr*)&peerAddr, &peerAddrLen)) < 0)  { // sockfd에서 오는 데이터를 msg에 저장
																												// 해당 데이터 보낸 주소가 peerAddr에 저장됨.
		perror("recvfrom");
		exit(1);
	}
	printf("Received reply: %s\n", msg.data);

	close(sockfd);
}
