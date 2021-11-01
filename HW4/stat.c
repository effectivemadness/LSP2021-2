#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>

int main(int argc, char *argv[])
{
	struct stat	statbuf;
	char		*mode;
	int			i;

	for (i = 1 ; i < argc ; i++)  {
		printf("%s: ", argv[i]);
		if (lstat(argv[i], &statbuf))  { // argv[i] 의 파일 상태 받아오기
			perror("lstat");
			continue;
		}
		if (S_ISREG(statbuf.st_mode)) // 일반 파일일때
			mode = "regular";
		else if (S_ISDIR(statbuf.st_mode)) // 디렉터리일때 
			mode = "directory";
		else if (S_ISCHR(statbuf.st_mode)) // character 단위 장치일때
			mode = "character special";
		else if (S_ISBLK(statbuf.st_mode)) // block 단위 장치일때
			mode = "block special";
		else if (S_ISFIFO(statbuf.st_mode)) // FIFO일때
			mode = "FIFO";
		else if (S_ISLNK(statbuf.st_mode)) // Symbolic link 일때
			mode = "symbolic link";
		else if (S_ISSOCK(statbuf.st_mode)) // 소켓 일때
			mode = "socket";
		printf("%s\n", mode); // 모드 출력 
		printf("\tst_mode = %d\n", statbuf.st_mode); // 권한 모드 출력
		printf("\tst_ino = %ld\n", statbuf.st_ino); // inode번호 출력
		printf("\tst_dev = %ld\n", statbuf.st_dev); // 속한 device ID
		printf("\tst_rdev = %ld\n", statbuf.st_rdev); // special file일 때 deivce ID
		printf("\tst_nlink = %ld\n", statbuf.st_nlink); // 하드링크 개수
		printf("\tst_uid = %d\n", statbuf.st_uid); // 소유하는 UID
		printf("\tst_gid = %d\n", statbuf.st_gid); // 소유하는 유저의 GID
		printf("\tst_size = %ld\n", statbuf.st_size); // 파일 바이트 수
		printf("\tst_atime = %ld\n", statbuf.st_atime); // 마지막 접근 시간 
		printf("\tst_mtime = %ld\n", statbuf.st_mtime); // 마지막 수정 시간
		printf("\tst_ctime = %ld\n", statbuf.st_ctime); // 마지막 상태 변경 시간
		printf("\tst_blksize = %ld\n", statbuf.st_blksize); // 파일시스템에서의 블럭 크기
		printf("\tst_blocks = %ld\n", statbuf.st_blocks); // 블럭 개수
	}
}
