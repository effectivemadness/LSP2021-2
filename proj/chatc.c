#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "chat.h"
#include <pthread.h>
#include <ctype.h>

#define	MAX_BUF		256

int		Sockfd;
pthread_t Tid[2];

/*===============================================================
[Function Name] : void	ServerInput(void *dummy)
[Description]   : 
    - 서버에서 보내는 소켓 입력 처리하는 쓰레드
	- 받은것을 stdout으로 출력하고, ""입력 때 종료
[Input]         :
    void *dummy : 쓰레드 더미
[Output]        :
    -
[Call By]       :	        
    Main()
[Calls]         :	        
    Nothing
[Given]         :
    Nothing
[Returns]       :
    Nothing
==================================================================*/
void
ServerInput(void *dummy)
{
	int		n;
	char	buf[MAX_BUF];
	if (pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL))  { // 쓰레드 취소 가능하도록 설정
		perror("pthread_setcancelstate");
		exit(1);
	}
	if (pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL))  { // 쓰레드 취소 비동기화되도록 설정
		perror("pthread_setcanceltype");
		exit(1);
	}
	while (1) // 무한루프
	{
		if ((n = recv(Sockfd, buf, MAX_BUF, 0)) < 0)  { // 서버의 입력
			perror("recv");
			exit(1);
		}
		if (n == 0)  { // 길이가 0이면 종료
			fprintf(stderr, "Server terminated.....\n");
			close(Sockfd); // 소켓 닫고
			exit(1); // 종료
		}
		printf("%s", buf); // 서버에서 온 값 출력
	}
}

/*===============================================================
[Function Name] : void	KeyboardInput(void *dummy)
[Description]   : 
    - 사용자의 키보드 입력 처리하는 쓰레드
	- 받은것을 서버와 연결된 소켓으로 전송
	- 처음 id 입력부분을 예외로 구현
[Input]         :
    void *dummy : 쓰레드 더미
[Output]        :
    -
[Call By]       :	        
    Main()
[Calls]         :	        
    Nothing
[Given]         :
    Nothing
[Returns]       :
    Nothing
==================================================================*/
void
KeyboardInput(void *dummy)
{
	int		n;
	char	buf[MAX_BUF];
	if (pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL))  { // 쓰레드 취소 가능하도록 설정
		perror("pthread_setcancelstate");
		exit(1);
	}
	if (pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL))  { // 쓰레드 취소 비동기화 되도록 설정
		perror("pthread_setcanceltype");
		exit(1);
	}

	printf("Enter ID: "); // id 입력 메세지
	fflush(stdout);
	fgets(buf, MAX_BUF, stdin); // id 입력받아 buf에 저장
	*strchr(buf, '\n') = '\0'; // 개행문자는 Null로 치환
	if (send(Sockfd, buf, strlen(buf)+1, 0) < 0)  { // id를 서버에 전송
		perror("send");
		exit(1);
	}
	printf("Press ^C to exit\n");
	while(1) // 무한 루프 돌면서
	{
		fgets(buf, MAX_BUF, stdin); // 입력받은 buf를 서버에 전송
		if ((n = send(Sockfd, buf, strlen(buf)+1, 0)) < 0)  {
			perror("send");
			exit(1);
		}
	}
}

/*===============================================================
[Function Name] : void	CloseClient()
[Description]   : 
    - 클라이언트 종료시 소켓 다 닫고 유닉스 소켓 파일들 정리하는 함수
	- 실행중인 쓰레드 취소 & join 하고 종료
[Input]         :
    -
[Output]        :
    -
[Call By]       :	        
    Main()
[Calls]         :	        
    Nothing
[Given]         :
    Nothing
[Returns]       :
    Nothing
==================================================================*/
void
CloseClient(int signo)
{
	if (pthread_cancel(Tid[0]))  { // 키보드 입력 쓰레드 취소
				perror("pthread_cancel");
				exit(1);
	}
	if (pthread_join(Tid[0], NULL) < 0)  { // join 대기
		perror("pthread_join");
		exit(1);
	}
	if (pthread_cancel(Tid[1]))  { // 서버 입력 쓰레드 취소
				perror("pthread_cancel");
				exit(1);
	}
    if (pthread_join(Tid[1], NULL) < 0)  { // join 대기
		perror("pthread_join");
		exit(1);
	}
	close(Sockfd); // 소켓 닫기
	printf("\nChat client terminated.....\n");

	exit(0); // 종료
}

/*===============================================================
[Program Name] : chatc
[Description]  : 
    - chats에 요청을 보내 접속, 다른 클라이언트들의 메세지를 쉘에 출력, 사용자의 입력을 서버에 전송하는 프로그램
[Input]        :
    stdin으로 사용자의 id, 메세지를 입력
[Output]       :
    shell stdout로 각 처리 여부를 출력
[Calls]        :	        
    -
[특기사항]     : 
	SIGINT : 열려있는 소켓을 닫고 종료
	소켓 입력, 키보드 입력을 쓰레드로 처리
==================================================================*/
int	main(int argc, char *argv[])
{
	struct sockaddr_in	servAddr;
	struct hostent		*hp;
	
	char	buf[MAX_BUF];


	if (argc != 2)  {
		fprintf(stderr, "Usage: %s ServerIPaddress\n", argv[0]); // 서버 주소 인자 없을시 처리
		exit(1);
	}

	if ((Sockfd = socket(PF_INET, SOCK_STREAM, 0)) < 0)  { // ipv4, TCP 사용하는 소켓 생성
		perror("socket");
		exit(1);
	}

	bzero((char *)&servAddr, sizeof(servAddr)); // servAddr 초기화
	servAddr.sin_family = PF_INET; // ipv4사용
	servAddr.sin_port = htons(SERV_TCP_PORT); //포트 주소 bigendian 으로 변환

	if (isdigit(argv[1][0]))  { // 숫자로 시작하면 ip 주소 간주, 주소를 빅 엔디안으로 변환
		servAddr.sin_addr.s_addr = inet_addr(argv[1]);
	}
	else  {
		if ((hp = gethostbyname(argv[1])) == NULL)  { // url을 ip로 변환
			fprintf(stderr, "Unknown host: %s\n", argv[1]);
			exit(1);
		}
		memcpy(&servAddr.sin_addr, hp->h_addr, hp->h_length); // 주소를 servAddr로 복사
	}

	if (connect(Sockfd, (struct sockaddr *) &servAddr, sizeof(servAddr)) < 0)  { // 서버와 연결
		perror("connect");
		exit(1);
	}

	signal(SIGINT, CloseClient); // 종료 인터럽트 핸들러 붙이기

	if (pthread_create(&Tid[0], NULL, (void*)KeyboardInput, (void*)NULL) < 0){ // 사용자 키보드 입력 처리 쓰레드 실행
		perror("pthread_create");
		exit(1);
	}
	if (pthread_create(&Tid[1], NULL, (void*)ServerInput, (void*)NULL) < 0){ // 서버 소켓 입력 처리 쓰레드 실행
		perror("pthread_create");
		exit(1);
	}

	if (pthread_join(Tid[0], NULL) < 0)  { // 사용자 키보드 입력 처리 쓰레드 join
		perror("pthread_join");
		exit(1);
	}
    if (pthread_join(Tid[1], NULL) < 0)  { // 서버 소켓 입력 처리 쓰레드 join
		perror("pthread_join");
		exit(1);
	}
}
