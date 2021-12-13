#define	MAX_FIFO_NAME	32

#define	SERV_FIFO	"./.fifo" // serv의 fifo 정의

typedef struct  {
	char	returnFifo[MAX_FIFO_NAME]; // reply 보낼 fifo 이름
	char	data[128]; // 데이터
}
	MsgType;
