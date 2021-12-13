#define	MY_ID			30
#define	SHM_KEY			(0x9000 + MY_ID)
#define	SHM_MODE		(SHM_R | SHM_W | IPC_CREAT)

#define	EMPTY_SEM_KEY	(0x5000 + MY_ID)
#define	FULL_SEM_KEY	(0x6000 + MY_ID)
#define	MUTEX_SEM_KEY	(0x7000 + MY_ID)

#define	NLOOPS			20

#define	MAX_BUF			2

typedef struct  {
	int		data;
}
	ItemType; // item type 구조체 선언

typedef struct  {
	ItemType	buf[MAX_BUF];
	int			in;
	int			out;
	int			counter;
}
	BoundedBufferType; // bounded buffer 구조체 선언

#define	SHM_SIZE	sizeof(BoundedBufferType)
