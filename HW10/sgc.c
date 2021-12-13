#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "sg.h"
#include <unistd.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <sys/uio.h>


int	main(int argc, char *argv[])
{
	int					sockfd, n;
	struct sockaddr_in	servAddr;
	MsgType				msg; // 메세지 구조체 타입 선언
	HeaderType			hdr; // 헤더 구조체 타입 선언
	struct iovec		iov[2]; // iovector 구조체 배열 선언

	if ((sockfd = socket(PF_INET, SOCK_STREAM, 0)) < 0) { // ipv4, TCP 프로토콜 사용하는 소켓 생성
		perror("socket");
		exit(1);
	}

	bzero((char *)&servAddr, sizeof(servAddr)); // servAddr 0으로 초기화
	servAddr.sin_family = PF_INET; // ipv4 사용
	servAddr.sin_addr.s_addr = inet_addr(SERV_HOST_ADDR); // 서버 주소 big endian으로 변경 후 저장
	servAddr.sin_port = htons(SERV_TCP_PORT); // 서버 포트 big endian으로 변경 후 저장

	if (connect(sockfd, (struct sockaddr *) &servAddr, sizeof(servAddr)) < 0)  { // socketfd, servAddr를 통해 서버에 접속
		perror("connect");
		exit(1);
	}

	strcpy(hdr.info, "REQST"); // 헤더 정보필드에 REQST 넣기
	msg.type = MSG_REQUEST; // 메세지 타입도 요청으로 넣기
	sprintf(msg.data, "This is a request from %d.", getpid());
	iov[0].iov_base = (char *)&hdr; // iovector[0] 내용 -> hdr 주소
	iov[0].iov_len = sizeof(hdr); // iovector[0] 길이 -> hdr 길이
	iov[1].iov_base = (char *)&msg; // iovector[1] 내용 -> msg 주소
	iov[1].iov_len = sizeof(msg); // iovector[1] 길이 -> msg 길이
	if (writev(sockfd, iov, 2) < 0)  { // sockfd로, iov 2개 작성
		perror("write");
		exit(1);
	}
	printf("Sent a request.....");
		
	if ((n = readv(sockfd, iov, 2)) < 0)  { // sockfd에서 iov 2개 읽어오기
		perror("read");
		exit(1);
	}
	printf("Received reply: %s(%s)\n", msg.data, hdr.info); // 응답 프린트

	close(sockfd);
}
