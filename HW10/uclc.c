#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/un.h>
#include "unix.h"


int	main(int argc, char *argv[])
{
	int					sockfd, n, servAddrLen, myAddrLen, peerAddrLen; 
	struct sockaddr_un	servAddr, myAddr, peerAddr;
	MsgType				msg;

	if ((sockfd = socket(PF_UNIX, SOCK_DGRAM, 0)) < 0)  { // UNIX 로컬 + UDP 프로토콜 사용하는 소켓 생성
		perror("socket");
		exit(1);
	}

	bzero((char *)&myAddr, sizeof(myAddr)); // myAddr 0으로 초기화
	myAddr.sun_family = PF_UNIX; // UNIX 로컬 프로토콜 설정
	sprintf(myAddr.sun_path, ".unix-%d", getpid()); // 주소를 현재 프로세스의 pid를 붙인 str로 설정
	myAddrLen = strlen(myAddr.sun_path) + sizeof(myAddr.sun_family); // myAddr 주소 길이 설정

	if (bind(sockfd, (struct sockaddr*)&myAddr, myAddrLen) < 0)  { // myaddr와 sockfd bind
		perror("bind");
		exit(1);
	}

	bzero((char *)&servAddr, sizeof(servAddr)); // servAddr 0으로 초기화
	servAddr.sun_family = PF_UNIX; // UNIX 로컬 프로토콜 설정
	strcpy(servAddr.sun_path, UNIX_DG_PATH); // 미리 정의해둔 Path로 주소 path 설정
	servAddrLen = strlen(servAddr.sun_path) + sizeof(servAddr.sun_family); // servAddr 길이 설정

	msg.type = MSG_REQUEST; // 요청으로 타입 설정
	sprintf(msg.data, "This is a request from %d.", getpid());
	if (sendto(sockfd, &msg, sizeof(msg), 0, (struct sockaddr*)&servAddr, servAddrLen) < 0)  { // servAddr로 sockfd를 통해 msg를 sizeof(msg)만큼 전송
		perror("sendto");
		exit(1);
	}
	printf("Sent a request.....");
		
	peerAddrLen = sizeof(peerAddr);
	if ((n = recvfrom(sockfd, &msg, sizeof(msg), 0, (struct sockaddr*)&peerAddr, &peerAddrLen)) < 0)  { // sockfd로 온 데이터 msg에 sizeof(msg)만큼 저장, 보낸 주소는 peerAddr에 저장.
		perror("recvfrom");
		exit(1);
	}
	printf("Received reply: %s\n", msg.data);

	close(sockfd); // 소켓 닫기
	if (remove(myAddr.sun_path) < 0)  { // path에 있는 파일 제거
		perror("remove");
		exit(1);
	}
}
