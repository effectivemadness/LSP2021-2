#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include "msg.h"
#include <stdlib.h>
#include <unistd.h>

#define	MSG_MODE	(0600 | IPC_CREAT)


int	main()
{
	int		msqid, n;
	MsgType	msg;


	if ((msqid = msgget(MSG_KEY, MSG_MODE)) < 0)  { // 메세지 큐 생성
		perror("msgget");
		exit(1);
	}

	if ((n = msgrcv(msqid, (char*)&msg, sizeof(msg.data), 0, 0)) < 0)  { // msg 크기만큼 메세지 큐에서 값 읽어오기
		perror("msgrcv");
		exit(1);
	}
	printf("Received request: %s.....", msg.data); // 받아온 데이터 출력

	msg.type = MSG_REPLY; // 답장으로 설정
	sprintf(msg.data, "This is a reply from %d.", getpid()); // 현재 PID에서 보내는 내용 작성
	if (msgsnd(msqid, (char*)&msg, sizeof(msg.data), 0) < 0)  { // 메세지 큐에 값 작성
		perror("msgsnd");
		exit(1);
	}
	printf("Replied.\n");

	sleep(1);

	/* Remove message queue */
	if (msgctl(msqid, IPC_RMID, 0) < 0)  { // 종료 전에 메세지 큐 제거
		perror("msgctl");
		exit(1);
	}
}
