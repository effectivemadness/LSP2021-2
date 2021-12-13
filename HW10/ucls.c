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
CloseServer() // 서버 종료 인터럽트 핸들러
{
	close(Sockfd); // 소켓 닫기
	if (remove(UNIX_DG_PATH) < 0)  { // path에 있는 파일 제거
		perror("remove");
	}

	printf("\nUNIX-domain Connection-Less Server exit.....\n");

	exit(0);
}


int	main(int argc, char *argv[])
{
	int					servAddrLen, cliAddrLen, n; // 서버주소 길이, 클라이언트 주소 길이, 읽은 길이 n
	struct sockaddr_un	cliAddr, servAddr; // 클라이언트 주소, 서버 주소
	MsgType				msg; // 메세지 구조체 타입

	signal(SIGINT, CloseServer); // 종료시 실행될 함수 붙이기

	if ((Sockfd = socket(PF_UNIX, SOCK_DGRAM, 0)) < 0)  { // UNIX 로컬 + UDP 프로토콜 사용하는 소켓 생성
		perror("socket");
		exit(1);
	}

	bzero((char *)&servAddr, sizeof(servAddr)); // servAddr 0으로 초기화
	servAddr.sun_family = PF_UNIX; // UNIX 로컬 프로토콜 설정
	strcpy(servAddr.sun_path, UNIX_DG_PATH); // 미리 정의해둔 Path로 주소 path 설정
	servAddrLen = strlen(servAddr.sun_path) + sizeof(servAddr.sun_family); // servAddr 길이 설정
	
	if (bind(Sockfd, (struct sockaddr*)&servAddr, servAddrLen) < 0)  { // servAddr와 sockfd bind
		perror("bind");
		exit(1);
	}

	printf("UNIX-domain Connection-Less Server started.....\n");

	cliAddrLen = sizeof(cliAddr);
	while (1)  {
		if (recvfrom(Sockfd, &msg, sizeof(msg), 0, (struct sockaddr*)&cliAddr, &cliAddrLen) < 0)  { // sockfd로 온 데이터 msg에 sizeof(msg)만큼 저장, 보낸 주소는 cliAddr에 저장.
			perror("recvfrom");
			exit(1);
		}
		printf("Received request: %s.....", msg.data);

		msg.type = MSG_REPLY; // 응답으로 msgtype 설정
		sprintf(msg.data, "This is a reply from %d.", getpid()); // msg 데이터 설정
		if (sendto(Sockfd, &msg, sizeof(msg), 0, (struct sockaddr*)&cliAddr, cliAddrLen) < 0)  { // servAddr로 sockfd를 통해 msg를 sizeof(msg)만큼 전송
			perror("sendto");
			exit(1);
		}
		printf("Replied.\n");
	}
}
