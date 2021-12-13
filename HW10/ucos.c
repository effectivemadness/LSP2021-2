#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/un.h>
#include <signal.h>
#include "unix.h"


int	Sockfd;

void
CloseServer()
{
	close(Sockfd);
	if (remove(UNIX_STR_PATH) < 0)  {
		perror("remove");
	}

	printf("\nUNIX-domain Connection-Oriented Server exit.....\n");

	exit(0);
}


int	main(int argc, char *argv[])
{
	int					newSockfd, servAddrLen, cliAddrLen, n;
	struct sockaddr_un	cliAddr, servAddr;
	MsgType				msg;

	signal(SIGINT, CloseServer);

	if ((Sockfd = socket(PF_UNIX, SOCK_STREAM, 0)) < 0)  { // PF_UNIX : 로컬 유닉스 프로토콜, SOCK_STREAM : TCP 프로토콜 사용하는 소켓 생성
		perror("socket");
		exit(1);
	}

	bzero((char *)&servAddr, sizeof(servAddr)); // servAddr 0으로 초기화
	servAddr.sun_family = PF_UNIX; // 프로토콜 = UNIX 
	strcpy(servAddr.sun_path, UNIX_STR_PATH); // 소켓 path
	servAddrLen = strlen(servAddr.sun_path) + sizeof(servAddr.sun_family); // servAddr 길이 설정

	if (bind(Sockfd, (struct sockaddr*)&servAddr, servAddrLen) < 0)  { // sockfd 와 서버 주소 bind.
		perror("bind");
		exit(1);
	}

	listen(Sockfd, 5); // sockfd에서 listen; 최대 5개의 client 대기 가능

	printf("UNIX-domain Connection-Oriented Server started.....\n");

	cliAddrLen = sizeof(cliAddr); // cliAddr의 길이 저장
	while (1)  {
		newSockfd = accept(Sockfd, (struct sockaddr*)&cliAddr, &cliAddrLen); // 연결요청된것 accept 후 newsockfd에 fd 저장
		if (newSockfd < 0)  {
			perror("accept");
			exit(1);
		}
		
		if ((n = read(newSockfd, (char*)&msg, sizeof(msg))) < 0)  { // 클라이언트가 보낸 값 읽기
			perror("read");
			exit(1);
		}
		printf("Received request: %s.....", msg.data);

		msg.type = MSG_REPLY; // 응답으로 msgtype 변경
		sprintf(msg.data, "This is a reply from %d.", getpid()); // 메세지 데이터 저장
		if (write(newSockfd, (char*)&msg, sizeof(msg)) < 0)  { // 클라이언트로 데이터 전송
			perror("write");
			exit(1);
		}
		printf("Replied.\n");

		close(newSockfd); // 소켓 닫기 
	}
}
