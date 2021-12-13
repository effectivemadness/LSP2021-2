#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "tcp.h"
#include <stdlib.h>
#include <unistd.h>
#include <strings.h>
#include <ctype.h>


int	main(int argc, char *argv[])
{
	int					sockfd, n; // 소켓fd, 읽은 비트수 변수
	struct sockaddr_in	servAddr; // 서버 주소
	MsgType				msg; // 메세지
	struct hostent		*hp; // hostentry 구조체

	if (argc != 2)  { // 인자가 1개만 들어오지 않았을 경우
		fprintf(stderr, "Usage: %s IPaddress\n", argv[0]);
		exit(1);
	}

	if ((sockfd = socket(PF_INET, SOCK_STREAM, 0)) < 0)  { // ipv4, TCP 사용하는 소켓 생성
		perror("socket");
		exit(1);
	}

	bzero((char *)&servAddr, sizeof(servAddr));  // servAddr 0으로 초기화
	servAddr.sin_family = PF_INET; // ipv4 사용
	servAddr.sin_port = htons(SERV_TCP_PORT); // tcp 포트 빅엔디안으로 변경후 저장

	if (isdigit(argv[1][0]))  { // 숫자로 시작하는 인자일 경우 = ip주소로 간주
		servAddr.sin_addr.s_addr = inet_addr(argv[1]); // 주소스트링을 빅엔디안 주소로 변경 후 저장
	}
	else  { // URL로 간주
		if ((hp = gethostbyname(argv[1])) == NULL)  { // URL을 DNS에 쿼리, ip 받아오기
			fprintf(stderr, "Unknown host: %s\n", argv[1]);
			exit(1);
		}
		memcpy(&servAddr.sin_addr, hp->h_addr, hp->h_length); // h_addr을 h_length만큼 sin_addr로 복사
	}

	if (connect(sockfd, (struct sockaddr*)&servAddr, sizeof(servAddr)) < 0)  { // servAddr로 연결
		perror("connect");
		exit(1);
	}

	msg.type = MSG_REQUEST; // 요청 타입 설정
	sprintf(msg.data, "This is a request from %d.", getpid());
	if (write(sockfd, &msg, sizeof(msg)) < 0)  { // sockfd로 msg 작성
		perror("write");
		exit(1);
	}
	printf("Sent a request.....");
		
	if ((n = read(sockfd, &msg, sizeof(msg))) < 0)  { // sockfd로 온 값 msg에 작성
		perror("read");
		exit(1);
	}
	printf("Received reply: %s\n", msg.data); // 메세지 출력

	close(sockfd); // 소켓 닫기
}
