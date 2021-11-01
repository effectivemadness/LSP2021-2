#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
	int		i;

	if (argc == 1)  {
		fprintf(stderr, "Usage: %s filename\n", argv[0]);
		exit(1);
	}

	for (i = 1 ; i < argc ; i++)  {
		if (remove(argv[i]))  {
			// 인자로 받은 파일이름들을 순회하면서 remove() 호출.
			// 해당 파일이 삭제됨.
			perror("remove");
			exit(1);
		}
	}
}
