#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include "msg.h"
#include <stdlib.h>
#include <unistd.h>

#define	MSG_MODE	(0600)


int	main()
{
	int		msqid, n; // 메세지 큐 id, 읽은 길이 저장 변수 
	MsgType	msg; // 주고받을 메세지 타입


	if ((msqid = msgget(MSG_KEY, MSG_MODE)) < 0)  { //메세지 큐 생성
		perror("msgget");
		exit(1);
	}

	msg.type = MSG_REQUEST; // 요청 메세지 정의
	sprintf(msg.data, "This is a request from %d.", getpid()); // 메세지 내용 저장
	if (msgsnd(msqid, (char *)&msg, sizeof(msg.data), 0) < 0)  { // 메세지 큐로 메세지 작성
		perror("msgsnd");
		exit(1);
	}
	printf("Sent a request.....");

	if ((n = msgrcv(msqid, (char *)&msg, sizeof(msg.data), MSG_REPLY, 0)) < 0)  { // 메세지 큐에서 메세지 받기
		perror("msgrcv");
		exit(1);
	}
	printf("Received reply: %s\n", msg.data); // 메세지 출력
}
