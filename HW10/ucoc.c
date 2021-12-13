#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/un.h>
#include "unix.h"


int	main(int argc, char *argv[])
{
	int					sockfd, n, servAddrLen; // 소켓의 fd, 서버 주소 길이 저장 변수
	struct sockaddr_un	servAddr;
	MsgType				msg;

	if ((sockfd = socket(PF_UNIX, SOCK_STREAM, 0 )) < 0)  { // PF_UNIX : 로컬 유닉스 프로토콜, SOCK_STREAM : TCP 프로토콜 사용하는 소켓 생성
		perror("socket");
		exit(1);
	}

	bzero((char *)&servAddr, sizeof(servAddr)); // servAddr 0으로 초기화
	servAddr.sun_family = PF_UNIX; // 프로토콜 = UNIX 
	strcpy(servAddr.sun_path, UNIX_STR_PATH); // 소켓 path
	servAddrLen = strlen(servAddr.sun_path) + sizeof(servAddr.sun_family); // servAddr 길이 설정

	if (connect(sockfd, (struct sockaddr*)&servAddr, servAddrLen))  { // 서버주소로 sockfd 연결
		perror("connect");
		exit(1);
	}

	msg.type = MSG_REQUEST; // 요청 타입으로 설정
	sprintf(msg.data, "This is a request from %d.", getpid()); // 데이터 저장
	if (write(sockfd, (char*)&msg, sizeof(msg)) < 0)  { // sockfd로 msg 작성
		perror("write");
		exit(1);
	}
	printf("Sent a request.....");
		
	if (read(sockfd, (char*)&msg, sizeof(msg)) < 0)  { // sockfd로 온 값 msg에 저장
		perror("read");
		exit(1);
	}
	printf("Received reply: %s\n", msg.data); // 응답 출력

	close(sockfd);
}
