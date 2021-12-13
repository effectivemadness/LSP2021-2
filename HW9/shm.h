#define	MY_ID		30 // lsp30
#define	SHM_KEY		(0x9000 + MY_ID)
#define	SHM_SIZE	1024
#define	SHM_MODE	(SHM_R | SHM_W | IPC_CREAT)

#define	SEM_1_KEY	(0x5000 + MY_ID)
#define	SEM_2_KEY	(0x6000 + MY_ID)
