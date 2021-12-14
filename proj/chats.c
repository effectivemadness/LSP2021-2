#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include "chat.h"


#define	DEBUG

#define	MAX_CLIENT	5
#define	MAX_ID		32
#define	MAX_BUF		256

typedef	struct  {
	int			sockfd;
	int			inUse;
	pthread_t	tid;
	char		uid[MAX_ID];
}
	ClientType;

int				Sockfd;

ClientType		Client[MAX_CLIENT];

/*===============================================================
[Function Name] : int	GetID()
[Description]   : 
    - Client[] 전역변수에서 사용중이 아닌 id를 사용중으로 바꾸고, 해당 id 리턴
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
    int i; // 할당된 id 리턴
==================================================================*/
int
GetID()
{
	int	i;

	for (i = 0 ; i < MAX_CLIENT ; i++)  { // 클라이언트 갯수만큼 loop
		if (! Client[i].inUse)  { // 사용중인 클라이언트 아니면 
			Client[i].inUse = 1; // 할당
			return i; // 해당 id 리턴
		}
	}
}

/*===============================================================
[Function Name] : void	SendToOtherClients(int id, char *buf)
[Description]   : 
    - 입력받은 클라이언트 외의 다른 클라이언트에 buf 전송하는 함수
[Input]         :
    int id; // 전송하는 클라이언트; 이 클라이언트에는 buf 전송하지 않음
	char *buf; // 전송할 문자열
[Output]        :
    -
[Call By]       :	        
    int	ProcessClient(int id)
[Calls]         :	        
    Nothing
[Given]         :
    Nothing
[Returns]       :
    Nothing
==================================================================*/
void
SendToOtherClients(int id, char *buf)
{
	int		i;
	char	msg[MAX_BUF+MAX_ID];

	sprintf(msg, "%s> %s", Client[id].uid, buf); // 메세지 만들기
#ifdef	DEBUG
	printf("%s", msg);
	fflush(stdout);
#endif

	for (i = 0 ; i < MAX_CLIENT ; i++)  { // 클라이언트 최대 개수만큼 루프
		if (Client[i].inUse && (i != id))  { // 연결되어있으면서 보내는 클라가 아니면
			if (send(Client[i].sockfd, msg, strlen(msg)+1, 0) < 0)  { // 전송
				perror("send");
				exit(1);
			}
		}
	}

}

/*===============================================================
[Function Name] : int	ProcessClient(int id)
[Description]   : 
    - 해당 id 클라이언트에서 보낸 값을 읽어 다른 클라이언트에 전송
[Input]         :
    int id; // 전송하는 클라이언트;
[Output]        :
    -
[Call By]       :	        
    Main()
[Calls]         :	        
    void	SendToOtherClients(int id, char *buf)
[Given]         :
    Nothing
[Returns]       :
    Nothing
==================================================================*/
int
ProcessClient(int id)
{
	char	buf[MAX_BUF];
	int		n;

	if ((n = recv(Client[id].sockfd, buf, MAX_BUF, 0)) < 0)  { // 클라이언트에서 받아오기
		perror("recv");
		exit(1);
	}
	if (n == 0)  { // 빈 문자 = 접속 종료
		printf("Client %d log-out(ID: %s).....\n", id, Client[id].uid); // 출력

		close(Client[id].sockfd); // 소켓 닫기
		Client[id].inUse = 0; // 사용중 아니게 처리

		strcpy(buf, "log-out.....\n");
		SendToOtherClients(id, buf); // 다른 클라들한테 logout 메세지 보내기

		return 1;
	}

	SendToOtherClients(id, buf); // 받은 메세지 다른 클라한테 보내기
	return 0;

}

/*===============================================================
[Function Name] : void	CloseServer()
[Description]   : 
    - 서버 종료시 소켓 다 닫고 정리하는 함수
	- 사용중인 client 소켓들을 확인 후 닫음
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
CloseServer()
{
	int		i;

	close(Sockfd); // 서버 소켓 닫기

	for (i = 0 ; i < MAX_CLIENT ; i++)  {
		if (Client[i].inUse)  { // 열려있는 소켓들 닫기
			close(Client[i].sockfd);
		}
	}

	printf("\nChat server terminated.....\n");

	exit(0);
}

/*===============================================================
[Program Name] : chats
[Description]  : 
    - TCP 요청으로 이루어진 채팅을 받아, 연결되어있는 클라이언트로 보내주는 프로그램
	- 각 클라이언트 새로운 접속과 요청은 select() 로 처리함
[Input]        :
    -
[Output]       :
    shell stdout로 각 처리 여부를 출력
[Calls]        :	        
    -
[특기사항]     : 
	SIGINT : 열려있는 소켓을 닫고 종료
==================================================================*/
int	main(int argc, char *argv[])
{
	int					newSockfd, cliAddrLen, id, count, n, one = 1;
	struct sockaddr_in	cliAddr, servAddr;
	fd_set	fdvar;

	signal(SIGINT, CloseServer); // 서버 종료시 인터럽트 핸들러 붙이기

	if ((Sockfd = socket(PF_INET, SOCK_STREAM, 0)) < 0)  { // ipv4, TCP 소켓 생성
		perror("socket");
		exit(1);
	}

	if (setsockopt(Sockfd, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one)) < 0)  { // 소켓 주소 재사용해 bind 가능하도록 하기.
		perror("setsockopt");
		exit(1);
	}

	bzero((char *)&servAddr, sizeof(servAddr)); // servAddr 0으로 초기화
	servAddr.sin_family = PF_INET; // ipv4 사용
	servAddr.sin_addr.s_addr = htonl(INADDR_ANY); // 어디서 출발한 요청이던 가능한 주소 사용, big endian으로 바꿔 저장
	servAddr.sin_port = htons(SERV_TCP_PORT); // 포트 번호 big endian으로 바꿔 저장

	if (bind(Sockfd, (struct sockaddr *) &servAddr, sizeof(servAddr)) < 0)  { // socketfd와 주소를 bind
		perror("bind");
		exit(1);
	}

	listen(Sockfd, 5); // 최대 5 클라이언트 대기 가능한 listen 시작

	printf("Chat server started.....\n");

	cliAddrLen = sizeof(cliAddr); // cliAddr 길이 저장
	while (1)  {
		FD_ZERO(&fdvar); // fdvar에 설정된 fd 전부 제거 = 초기화
		FD_SET(Sockfd, &fdvar); // listen 듣는 소켓 set
		for (int i = 0 ; i < MAX_CLIENT ; i++)  {
			if (Client[i].inUse) // 현재 접속중인 클라이언트 소켓 set
				FD_SET(Client[i].sockfd, &fdvar);
		}
		
		if ((count = select(MAX_CLIENT+5, &fdvar, (fd_set*)0, (fd_set*)0, (struct timeval*)0)) < 0) { // set 된 fd에서 읽기 가능할때까지 대기 & 개수 리턴
			perror("select");
			exit(1);
		}
		while (count) // count가 0이 아닐때까지 loop
		{
			if (FD_ISSET(Sockfd, &fdvar))
			{
				// 새 클라이언트
				newSockfd = accept(Sockfd, (struct sockaddr *) &cliAddr, &cliAddrLen); // 새 클라이언트 연결 설정
				if (newSockfd < 0)  {
					perror("accept");
					exit(1);
				}

				id = GetID(); // 빈 id 할당
				Client[id].sockfd = newSockfd; // fd 할당
				if ((n = recv(Client[id].sockfd, Client[id].uid, MAX_ID, 0)) < 0)  { // id 받아오기
					perror("recv");
					exit(1);
				}
				printf("Client %d log-in(ID: %s).....\n", id, Client[id].uid);
				count--;
				if (!count)
					break;
			}
			// 이미 있던 클라이언트 확인
			for (int i = 0 ; i < MAX_CLIENT ; i++)  {
				if (Client[i].inUse && FD_ISSET(Client[i].sockfd, &fdvar))  { // 클라이언트에서 보낸 내용이 있으면
					if (ProcessClient(i) == 1) // process 하기
						Client[i].inUse = 0; // disconnect한 클라면 inUse = 0으로 초기화
						
					count--;
					if (!count)
						break;
				}	
			}
		}
	}
}
