#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include "sg.h"
#include <unistd.h>
#include <stdlib.h>
#include <strings.h>
#include <sys/uio.h>
#include <string.h>


int	Sockfd;

void
CloseServer() // 종료 인터럽트 핸들러 함수
{
	close(Sockfd);
	printf("\nScatter/Gather TCP Server exit.....\n");

	exit(0);
}


int	main(int argc, char *argv[])
{
	int					newSockfd, cliAddrLen, n; // tcp sockfd, client 주소 길이, 읽은 길이 변수
	struct sockaddr_in	cliAddr, servAddr; // cliAddr, servAddr 주소 구조체
	MsgType				msg; // 메세지 구조체 타입 선언
	HeaderType			hdr; // 헤더 구조체 타입 선언
	struct iovec		iov[2]; // iovector 구조체 배열 선언

	signal(SIGINT, CloseServer); // 서버 종료 인터럽트 핸들러 

	if ((Sockfd = socket(PF_INET, SOCK_STREAM, 0)) < 0)  { // ipv4, TCP 사용하는 소켓 생성
		perror("socket");
		exit(1);
	}

	bzero((char *)&servAddr, sizeof(servAddr)); // servAddr 초기화
	servAddr.sin_family = PF_INET; // ipv4 프로토콜
	servAddr.sin_addr.s_addr = htonl(INADDR_ANY); // 아무곳에서 보낸 요청 허가하는 주소 사용 & big endian으로 변환 후 저장
	servAddr.sin_port = htons(SERV_TCP_PORT); // 서버 포트 big endian 으로 변환 후 저장

	if (bind(Sockfd, (struct sockaddr *) &servAddr, sizeof(servAddr)) < 0)  { // Sockfd와 servAddr bind
		perror("bind");
		exit(1);
	}

	listen(Sockfd, 5); // sockfd에서 최대 5개의 요청 대기하며 listen

	printf("Scatter/Gather TCP Server started.....\n");

	cliAddrLen = sizeof(cliAddr); // cliAddr 길이 저장
	while (1)  {
		newSockfd = accept(Sockfd, (struct sockaddr *) &cliAddr, &cliAddrLen); // 대기중인 요청 accept, 상대 주소는 cliAddr에 저장
		if (newSockfd < 0)  {
			perror("accept");
			exit(1);
		}
		
		iov[0].iov_base = (char *)&hdr; // iovector[0] 내용 -> hdr 주소
		iov[0].iov_len = sizeof(hdr); // iovector[0] 길이 -> hdr 길이
		iov[1].iov_base = (char *)&msg; // iovector[1] 내용 -> msg 주소
		iov[1].iov_len = sizeof(msg); // iovector[1] 길이 -> msg 길이
		if ((n = readv(newSockfd, iov, 2)) < 0)  { // newSockfd에서 ioVector 2개 읽어다가 iov에 저장
			perror("read");
			exit(1);
		}
		printf("Received request: %s(%s).....", msg.data, hdr.info); // 받은 요청 출력

		strcpy(hdr.info, "REPLY"); // hdr.info에 응답 표시
		msg.type = MSG_REPLY; // msg.type에 응답 표시
		sprintf(msg.data, "This is a reply from %d.", getpid()); // msg 내용 저장
		if (writev(newSockfd, iov, 2) < 0)  { // newSockfd에 iov 2개 묶어 전송
			perror("write");
			exit(1);
		}
		printf("Replied.\n");

		close(newSockfd); // 소켓 닫기
	}
}
