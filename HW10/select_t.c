#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/un.h>
#include <signal.h>
#include <sys/time.h>
#include <unistd.h>
#include "select.h"
#include <stdlib.h>
#include <pthread.h>

// 각 소켓들의 fd 전역변수
int	TcpSockfd;
int	UdpSockfd;
int	UcoSockfd;
int	UclSockfd;

/*===============================================================
[Function Name] : void	CloseServer()
[Description]   : 
    - 서버 종료시 소켓 다 닫고 유닉스 소켓 파일들 정리하는 함수
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
CloseServer() // 서버 종료 핸들러
{
	// 각 fd들 닫기
	close(TcpSockfd);
	close(UdpSockfd);
	close(UcoSockfd);
	close(UclSockfd);
	// 유닉스 소켓 파일 제거
	if (remove(UNIX_STR_PATH) < 0)  {
		perror("remove");
	}
	if (remove(UNIX_DG_PATH) < 0)  {
		perror("remove");
	}

	printf("\nServer daemon exit.....\n");

	exit(0);
}

/*===============================================================
[Function Name] : void	MakeTcpSocket()
[Description]   : 
    - TCP 소켓 생성 및 Listen 함수
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
MakeTcpSocket() // tcp 소켓 생성 & listen
{
	struct sockaddr_in	servAddr; // 서버 주소 구조체

	if ((TcpSockfd = socket(PF_INET, SOCK_STREAM, 0)) < 0)  { // ipv4, TCP 프로토콜 사용 소켓 생성
		perror("socket");
		exit(1);
	}

	bzero((char *)&servAddr, sizeof(servAddr)); // servAddr 0으로 초기화
	servAddr.sin_family = PF_INET; // ipv4 프로토콜 사용
	servAddr.sin_addr.s_addr = htonl(INADDR_ANY); // 아무곳에서 보내는 응답 다 듣는 설정, big endian으로 변경 후 저장
	servAddr.sin_port = htons(SERV_TCP_PORT); // 포트 big endian으로 저장

	if (bind(TcpSockfd, (struct sockaddr *) &servAddr, sizeof(servAddr)) < 0)  { // socketfd를 servAddr와 bind
		perror("bind");
		exit(1);
	}

	listen(TcpSockfd, 5); // 최대 5개 호스트 대기하는 listen 시작
}

/*===============================================================
[Function Name] : void	MakeUdpSocket()
[Description]   : 
    - UDP 소켓 생성 함수
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
MakeUdpSocket() // UDP 소켓 생성
{
	struct sockaddr_in	servAddr; // 서버 주소 구조체

	if ((UdpSockfd = socket(PF_INET, SOCK_DGRAM, 0)) < 0)  { // ipv4, TCP 프로토콜 사용 소켓 생성
		perror("socket");
		exit(1);
	}

	bzero((char *)&servAddr, sizeof(servAddr)); // servAddr 0으로 초기화
	servAddr.sin_family = PF_INET; // ipv4 프로토콜 사용
	servAddr.sin_addr.s_addr = htonl(INADDR_ANY); // 아무곳에서 보내는 응답 다 듣는 설정, big endian으로 변경 후 저장
	servAddr.sin_port = htons(SERV_UDP_PORT); // 포트 big endian으로 저장

	if (bind(UdpSockfd, (struct sockaddr *) &servAddr, sizeof(servAddr)) < 0)  { // socketfd를 servAddr와 bind
		perror("bind");
		exit(1);
	}

}

/*===============================================================
[Function Name] : void	MakeUcoSocket()
[Description]   : 
    - Unix + connection oriented 소켓 생성 및 Listen 함수
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
MakeUcoSocket() // Unix Connection-oriented 소켓 생성 & listen
{
	struct sockaddr_un	servAddr;
	int					servAddrLen;

	if ((UcoSockfd = socket(PF_UNIX, SOCK_STREAM, 0)) < 0)  { // UNIX 소켓 + connection orinted 프로토콜 소켓 생성
		perror("socket");
		exit(1);
	}

	bzero((char *)&servAddr, sizeof(servAddr)); // servAddr 0으로 초기화
	servAddr.sun_family = PF_UNIX; // PF_UNIX 프로토콜 설정
	strcpy(servAddr.sun_path, UNIX_STR_PATH); // 통신에 사용될 파일시스템 내 경로
	servAddrLen = strlen(servAddr.sun_path) + sizeof(servAddr.sun_family); // servAddr 길이 저장

	if (bind(UcoSockfd, (struct sockaddr *) &servAddr, servAddrLen) < 0)  { // socketfd를 servAddr와 bind
		perror("bind");
		exit(1);
	}

	listen(UcoSockfd, 5); // 최대 5개 클라이언트 대기하는 listen 시작

}

/*===============================================================
[Function Name] : void	MakeUclSocket()
[Description]   : 
    - Unix + Connectionless 소켓 생성 함수
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
MakeUclSocket() // Unix Connectionless 소켓 생성
{
	struct sockaddr_un	servAddr;
	int					servAddrLen;

	if ((UclSockfd = socket(PF_UNIX, SOCK_DGRAM, 0)) < 0)  { // UNIX 소켓 + connectionless 프로토콜 사용하는 소켓 생성
		perror("socket");
		exit(1);
	}

	bzero((char *)&servAddr, sizeof(servAddr)); // servAddr 0으로 초기화
	servAddr.sun_family = PF_UNIX; // PF_UNIX 프로토콜 설정
	strcpy(servAddr.sun_path, UNIX_DG_PATH); // 통신에 사용될 파일시스템 내 경로 설정
	servAddrLen = strlen(servAddr.sun_path) + sizeof(servAddr.sun_family); // servAddr 길이 저장

	if (bind(UclSockfd, (struct sockaddr *) &servAddr, servAddrLen) < 0)  { // socketfd를 servAddr와 bind
		perror("bind");
		exit(1);
	}

}

/*===============================================================
[Function Name] : void	ProcessTcpRequest(void *dummy)
[Description]   : 
    - 무한반복하며 TCP 요청 처리하는 함수
[Input]         :
    - void *dummy // 쓰레드 생성시 dummy로 들어오는 인자
[Output]        :
    - stdout으로 받은 요청과 응답 처리 여부 출력
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
ProcessTcpRequest(void *dummy) // TCP 요청 처리
{
	int					newSockfd, cliAddrLen, n; // 소켓fd 저장할 변수들
	struct sockaddr_in	cliAddr; // 클라 주소 저장 구조체
	MsgType				msg; 
    while(1)
    {
        cliAddrLen = sizeof(cliAddr); // client 주소 구조체 길이 저장
        newSockfd = accept(TcpSockfd, (struct sockaddr *) &cliAddr, &cliAddrLen); // 대기중인 요청 accept, cliAddr에 주소 저장
        if (newSockfd < 0)  {
            perror("accept");
            exit(1);
        }
            
        if ((n = read(newSockfd, (char *)&msg, sizeof(msg))) < 0)  { // client가 보낸 데이터 읽기
            perror("read");
            exit(1);
        }
        printf("Received TCP request: %s.....", msg.data); // 출력

        msg.type = MSG_REPLY; // 응답으로 msg type 설정
        sprintf(msg.data, "This is a reply from %d.", getpid());
        if (write(newSockfd, (char *)&msg, sizeof(msg)) < 0)  { // client로 데이터 전송
            perror("write");
            exit(1);
        }
        printf("Replied.\n");

        close(newSockfd); // 새로 만든 클라이언트 fd 닫기
    }
}

/*===============================================================
[Function Name] : void	ProcessUdpRequest(void *dummy)
[Description]   : 
    - 무한반복하며 UDP 요청 처리하는 함수
[Input]         :
    - void *dummy // 쓰레드 생성시 dummy로 들어오는 인자
[Output]        :
    - stdout으로 받은 요청과 응답 처리 여부 출력
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
ProcessUdpRequest(void *dummy) // UDP 요청 처리
{
	int					cliAddrLen, n;
	struct sockaddr_in	cliAddr;
	MsgType				msg;
    while (1)
    {
        cliAddrLen = sizeof(cliAddr); // client 주소 구조체 길이 저장
        if ((n = recvfrom(UdpSockfd, (char *)&msg, sizeof(msg), // udpsockfd에서 msg만큼 읽어오기
                    0, (struct sockaddr *)&cliAddr, &cliAddrLen)) < 0)  { // 보낸곳 주소는 cliAddr에 저장
            perror("recvfrom");
            exit(1);
        }
        printf("Received UDP request: %s.....", msg.data);

        msg.type = MSG_REPLY; // 응답으로 msg type 설정
        sprintf(msg.data, "This is a reply from %d.", getpid()); // msg 내용 저장
        if (sendto(UdpSockfd, (char *)&msg, sizeof(msg), // udpsockfd로 msg만큼, cliAddr를 통해 전송
                    0, (struct sockaddr *)&cliAddr, cliAddrLen) < 0)  {
            perror("sendto");
            exit(1);
        }
        printf("Replied.\n");
    }
}

/*===============================================================
[Function Name] : void	ProcessUcoRequest(void *dummy)
[Description]   : 
    - 무한반복하며 Unix + connection oriented 요청 처리하는 함수
[Input]         :
    - void *dummy // 쓰레드 생성시 dummy로 들어오는 인자
[Output]        :
    - stdout으로 받은 요청과 응답 처리 여부 출력
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
ProcessUcoRequest(void *dummy) // UNIX + connection oriented protocol 응답
{
	int					newSockfd, cliAddrLen, n;
	struct sockaddr_un	cliAddr;
	MsgType				msg;
    while (1)
    {
        cliAddrLen = sizeof(cliAddr); // cliAddr 길이 저장
        newSockfd = accept(UcoSockfd, (struct sockaddr *) &cliAddr, &cliAddrLen); // 대기중인 client 연결 accept, 주소는 cliAddr에 저장
        if (newSockfd < 0)  {
            perror("accept");
            exit(1);
        }
            
        if ((n = read(newSockfd, (char *)&msg, sizeof(msg))) < 0)  { // newSockfd에서 msg 읽어오기
            perror("read");
            exit(1);
        }
        printf("Received UNIX-domain CO request: %s.....", msg.data);

        msg.type = MSG_REPLY; // 응답 type 설정
        sprintf(msg.data, "This is a reply from %d.", getpid()); // 응답 메세지 내용 설정
        if (write(newSockfd, (char *)&msg, sizeof(msg)) < 0)  { // newSockfd에 msg 작성
            perror("write");
            exit(1);
        }
        printf("Replied.\n");

        close(newSockfd); // 생성했던 newSockfd 닫기
    }
}

/*===============================================================
[Function Name] : void	ProcessUclRequest(void *dummy)
[Description]   : 
    - 무한반복하며 Unix + connectionless 요청 처리하는 함수
[Input]         :
    - void *dummy // 쓰레드 생성시 dummy로 들어오는 인자
[Output]        :
    - stdout으로 받은 요청과 응답 처리 여부 출력
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
ProcessUclRequest(void *dummy) // UNIX + connectionless protocol 응답
{
	int					cliAddrLen, n;
	struct sockaddr_un	cliAddr;
	MsgType				msg;
    while(1)
    {
   
        cliAddrLen = sizeof(cliAddr); // cliAddr 길이 저장
        if ((n = recvfrom(UclSockfd, (char *)&msg, sizeof(msg), // UclSockfd 에서 msg 에 내용 넣기 
                    0, (struct sockaddr *)&cliAddr, &cliAddrLen)) < 0)  { // 보낸 주소는 cliAddr로 저장
            perror("recvfrom");
            exit(1);
        }
        printf("Received UNIX-domain CL request: %s.....", msg.data);

        msg.type = MSG_REPLY; // 응답 type 설정
        sprintf(msg.data, "This is a reply from %d.", getpid()); // 응답 메세지 내용 설정
        if (sendto(UclSockfd, (char *)&msg, sizeof(msg), // UclSockfd로 msg 보내기
                    0, (struct sockaddr *)&cliAddr, cliAddrLen) < 0)  {
            perror("sendto");
            exit(1);
        }
        printf("Replied.\n");
         
    }
}

/*===============================================================
[Program Name] : select_t
[Description]  : 
    - TCP, UDP, Unix Connectionless, Unix Connection Oriented 소켓 서버를 쓰레드로 구현
    - 각 프로토콜 소켓을 생성 후, 소켓 응답을 쓰레드로 처리
[Input]        :
    -
[Output]       :
    shell stdout로 각 프로토콜 처리 여부를 출력
[Calls]        :	        
    -
[특기사항]     : 
	-
==================================================================*/
int	main(int argc, char *argv[])
{
	fd_set	fdvar;
    pthread_t	tid[4];

	signal(SIGINT, CloseServer); // 서버 종료 인터럽트 핸들러 붙이기

	// 소켓들 생성
	MakeTcpSocket();
	MakeUdpSocket();
	MakeUcoSocket();
	MakeUclSocket();

	printf("Server daemon started.....\n");

    if (pthread_create(&tid[0], NULL, (void *)ProcessTcpRequest, (void *)NULL) < 0)  { // TCP 요청 응답 쓰레드 생성
		perror("pthread_create");
		exit(1);
	}
    if (pthread_create(&tid[1], NULL, (void *)ProcessUdpRequest, (void *)NULL) < 0)  { // UDP 요청 응답 쓰레드 생성
		perror("pthread_create");
		exit(1);
	}
    if (pthread_create(&tid[2], NULL, (void *)ProcessUcoRequest, (void *)NULL) < 0)  { // UNIX Connetection Oriented 요청 응답 쓰레드 생성
		perror("pthread_create");
		exit(1);
	}
    if (pthread_create(&tid[3], NULL, (void *)ProcessUclRequest, (void *)NULL) < 0)  { // UNIX Connectionless 요청 응답 쓰레드 생성
		perror("pthread_create");
		exit(1);
	}


	if (pthread_join(tid[0], NULL) < 0)  { // TCP 요청 응답 쓰레드 종료 대기
		perror("pthread_join");
		exit(1);
	}
    if (pthread_join(tid[1], NULL) < 0)  { //  UDP 요청 응답 쓰레드 종료 대기
		perror("pthread_join");
		exit(1);
	}
    if (pthread_join(tid[2], NULL) < 0)  { // UNIX Connetection Oriented 요청 응답 쓰레드 종료 대기
		perror("pthread_join");
		exit(1);
	}
    if (pthread_join(tid[3], NULL) < 0)  { // UNIX Connectionless 요청 응답 쓰레드 종료 대기
		perror("pthread_join");
		exit(1);
	}


}
