#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>

#ifndef MAX // MAX매크로 정의되지 않은 상태이면,
#define MAX(a,b)  (((a) > (b)) ? (a) : (b)) // MAX 매크로 정의
#endif


/*===============================================================
[Function Name] : char *getUser(uid_t id)
[Description]   : 
    - uid를 받아 유저의 이름을 리턴
[Input]         :
    uid_t id		// 이름을 찾을 유저의 uid
[Output]        :
    char *getUser(uid_t id);   // uid를 사용해 찾은 유저 이름을 담은 char* 
[Call By]       :	        
    Main()
[Calls]         :	        
    Nothiong
[Given]         :
    Nothing
[Returns]       :
    uid로 참조한 유저의 이름을 담은 char*
==================================================================*/
char* getUser(uid_t id)
{
	struct passwd *pwd;
	pwd = getpwuid(id); // id를 통해 유저구하기
	return (pwd->pw_name); // 이름 리턴
}

/*===============================================================
[Function Name] : char *getGroup(gid_t id)
[Description]   : 
    - gid를 받아 해당 그룹의 이름을 리턴
[Input]         :
    uid_t id		// 이름을 찾을 그룹의 gid
[Output]        :
    char *getGroup(gid_t id);   // gid를 사용해 찾은 그룹 이름을 담은 char* 
[Call By]       :	        
    Main()
[Calls]         :	        
    Nothiong
[Given]         :
    Nothing
[Returns]       :
    gid로 참조한 그룹의 이름을 담은 char*
==================================================================*/
char* getGroup(gid_t id)
{
	struct group *g;
	g = getgrgid(id); // id를 통해 그룹 구하기 
	return (g->gr_name); // 이름 리턴
}

/*===============================================================
[Function Name] : char getFileType(mode_t mode)
[Description]   : 
    - mode bit를 받아 파일의 타입을 나타내는 문자를 리턴
[Input]         :
    mode_t mode		// lstat으로 찾은 파일의 mode bit
[Output]        :
    char getFileType(mode_t mode);   // modebit에서 찾은 파일의 타입 문자
[Call By]       :	        
    Main()
[Calls]         :	        
    Nothiong
[Given]         :
    Nothing
[Returns]       :
    modebit에서 찾은 파일의 타입 문자
==================================================================*/
char getFileType(mode_t mode)
{
	char tmp;
	if (S_ISREG(mode))
		tmp = '-'; // 일반 파일
	else if (S_ISDIR(mode))
		tmp = 'd'; // 디렉토리
	else if (S_ISCHR(mode))
		tmp = 'c'; // 캐릭터 디바이스
	else if (S_ISBLK(mode))
		tmp = 'b'; // 블럭 디바이스
	else if (S_ISFIFO(mode))
		tmp = 'p'; // FIFO 디바이스
	else if (S_ISLNK(mode))
		tmp = 'l'; // 링크
	else if (S_ISSOCK(mode))
		tmp = 's'; // 소켓
	return tmp;
}

typedef struct tabsize {
	size_t hl;
	size_t uname;
	size_t gname;
	size_t fsize;
} tabsize_t;

/*===============================================================
[Program Name] : mylsl
[Description]  : 
    - "ls -l" 명령어와 유사하게 동작하는 프로그램
    - 파일들의 목록, 크기, 링크개수, 권한 등을 출력
[Input]        :
    -
[Output]       :
    - 쉘 출력
[Calls]        :	        
    char getFileType(mode_t type) // 파일의 modebit를 받아 파일의 type를 리턴
    char *getUser(uid_t id) // uid를 받아 유저의 이름 문자열을 리턴
    char *getGroup(gid_t id) // gid를 받아 그룹의 이름 문자열을 리턴
==================================================================*/
int main()
{
	struct dirent	*dep;
	struct dirent	**reclist;
	struct stat	statbuf; // 파일 상태 저장하는 buf
	char mode[8][4] = {"---", "--x", "-w-", "-wx", "r--", "r-x", "rw-", "rwx"}; // 권한 표시하기 좋게 스트링 문자열 미리 선언
	int 	count; // 파일 레코드의 수 저장 변수
	tabsize_t tabsize; // 문자열별 공백 수 저장 구조체

	if ((count = scandir(".", &reclist, NULL, alphasort)) == -1) //열어서 알파벳순으로 정렬한 dirent 배열 리턴
	{  
		perror("scandir"); //에러 출력
		exit(0);
	}
	memset(&tabsize, 0, sizeof(tabsize)); // 공백 구조체 초기화
	for (int i = 0; i < count; i++) 
	{
		char buf[21]; //문자열 임시 버퍼
		dep = reclist[i];
		if (dep->d_name[0] == '.') // 닷파일은 제외
			continue;
		lstat(dep->d_name, &statbuf); // 파일 상태정보 읽어오기
		sprintf(buf, "%lu", statbuf.st_nlink); // 하드링크 수 스트링 생성
		tabsize.hl = MAX(tabsize.hl, strlen(buf)); // 최대 길이만큼 저장
		tabsize.uname = MAX(tabsize.uname, strlen(getUser(statbuf.st_uid))); // 유저이름 최대 길이만큼 저장
		tabsize.gname = MAX(tabsize.gname, strlen(getGroup(statbuf.st_gid))); // 그룹이름 최대 길이만큼 저장
		sprintf(buf, "%lu", statbuf.st_size); // 사이즈 스트링 생성
		tabsize.fsize = MAX(tabsize.fsize, strlen(buf)); // 최대 길이만큼 저장 
	}
	for (int i = 0; i < count; i++) { // 파일 레코드들 순회
		char formatbuf[25]; // 스트링 출력 포멧 버퍼
		dep = reclist[i];
		if (dep->d_name[0] == '.') // 닷파일 제외
			continue;
		lstat(dep->d_name, &statbuf); // 파일 상태정보 읽어오기

		printf("%c", getFileType(statbuf.st_mode)); // 파일 타입 출력
	
		int mask = statbuf.st_mode; // mode bit 저장 
		char permbuf[4]; // 권한 문자열 저장할 버퍼 선언
		strcpy(permbuf, mode[(S_IRWXU & mask) >> 6]); // 유저 권한의 스트링 저장
		if (S_ISUID & mask) // UID 설정되어있는지 확인
		{
			if(S_IXUSR & mask) // 실행권한 있는 경우 
				permbuf[2] = 's'; // 소문자 s 표시
			else // 실행권한 없는 경우
				permbuf[2] = 'S'; // 대문자 s 표시
		}
		printf("%s", permbuf); // 유저 권한 출력
		strcpy(permbuf, mode[(S_IRWXG & mask) >> 3]); // 그룹 권한 스트링 저장
		printf("%s", permbuf); // 그룹 권한 출력 
		strcpy(permbuf, mode[(S_IRWXO & mask)]); // Others 권한 스트링 저장
		printf("%s ", permbuf); // Others 권한 출력 
		
		sprintf(formatbuf, "%%%lulu ", tabsize.hl); // 하드링크 개수 출력 포멧 문자열 저장
		printf(formatbuf, statbuf.st_nlink); // 하드링크 개수 출력
			
		sprintf(formatbuf, "%%%lus ", tabsize.uname); // 유저이름 출력 포멧 문자열 저장
		printf(formatbuf, getUser(statbuf.st_uid)); // 유저이름 출력 
		sprintf(formatbuf, "%%%lus ", tabsize.gname); // 그룹이름 출력 포멧 문자열 저장
		printf(formatbuf, getGroup(statbuf.st_gid)); // 그룹이름 출력
		sprintf(formatbuf, "%%%lulu ", tabsize.fsize); // 파일 크기 출력 포멧 문자열 저장
		printf(formatbuf, statbuf.st_size); // 파일 크기 출력
		
		struct tm *modtime; // tm 구조체 만들기
		modtime = localtime(&statbuf.st_mtime); // 수정 시간 linuxtime 에서 tm 구조체로 변환
		char timebuff[15]; // 시간 저장할 버퍼
		strftime(timebuff, 15, "%b %d %H:%M", modtime); // 실습 서버 ls 출력에 맞게 시간 포멧 지정
		printf("%s ", timebuff); // 수정 시간 출력
		printf("%s\n", dep->d_name); // 파일 이름 출력
	}
	for (int i = 0; i < count; i++)
		free(reclist[i]); // 파일 리스트 배열내용 free
	free(reclist); // 파일 리스트 free
}
