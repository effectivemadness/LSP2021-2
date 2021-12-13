#define	UNIX_STR_PATH	"./.unix-str"
#define	UNIX_DG_PATH	"./.unix-dg"

#define	MSG_REQUEST		1
#define	MSG_REPLY		2
#include <stdlib.h>
#include <unistd.h>
#include <strings.h>

typedef struct  {
	int		type;
	char	data[128];
}
	MsgType;
