#define	MY_ID			30 //lsp30
#define	MSG_KEY			(0x8000 + MY_ID)

#define	MSG_REQUEST		1
#define	MSG_REPLY		2

typedef struct  {
	long	type; // 데이터 타입
	char	data[128]; // 데이터
}
	MsgType;
