#include <stdio.h>
#include <string.h>

// 과제 1
int
strlen_p(char *str)
{
	int	len = 0;

	while (*str)  { //str가 가르키는 문자가 '\0'이 아닌 동안
		len++; //길이 저장 배열 증가
		str++; //str 한칸 이동
	}
	return len;
}

int
strlen_a(char str[])
{
	int i;

	for (i = 0 ; str[i] != '\0' ; i++) //str[i] 가 '\0'이 아닌 동안 루프
		;
	return i;
}

void
strcpy_p(char *dst, char *src)
{
	while (*src)  { //*src가 '\0' 이 아닌 동안 루프
		*dst = *src; //한 글자씩 dst에 복사하고
		dst++; // dst와 src는 한칸씩 이동
		src++;
	} 
	*dst = *src; // 마지막 널문자 찍어준다.
}

void
strcpy_a(char dst[], char src[])
{
	int i;

	for (i = 0 ; src[i] != '\0' ; i++) // src[i] 가 i 가 아닌 동안 루프
		dst[i] = src[i]; // 한 글자씩 복사
	dst[i] = src[i]; // 마지막 널 문자 찍어준다.
}

void
strcat_p(char *dst, char *src)
{
	while (*dst++) //dst의 끝까지 이동
		;
	dst--; //널문자를 지나서 한 칸 왔으므로 한칸 빼준다.
	while (*src)  { //*src가 '\0'이 아닌 동안 루프
		*dst++ = *src++; // 한 글자씩 복사 후 한칸씩 이동
	}
	*dst = *src; // 마지막 널 문자 찍어준다.
}

void
strcat_a(char dst[], char src[])
{
	int i, j;

	for (i = 0 ; dst[i] != '\0' ; i++) // dst[i]가 널문자일때까지 루프
		;
	for (j = 0 ; src[j] != '\0' ; j++) 
		dst[i+j] = src[j]; // 한 글자씩 복사해준다
	dst[i+j] = '\0'; // 마지막 널 문자 찍어준다.
}

// 과제 2

/*===============================================================
[Function Name] : int cutoff_diff(char c1, char c2)
[Description]   :
    - 두 문자의 차이를 -1, 0, 1로 리턴한다.
[Input]         :
    char c1;	// 비교할 첫번째 문자
    char c2;	// 비교할 두번째 문자
[Output]        :
    int cutoff_diff(char c1, char c2);	// c1이 c2보다 크면 1, 작으면 -1, 같으면 0 리턴
[Call By]       :
    strcmp_p()
    strcmp_a()
[Calls]         :
    Nothiong
[Given]         :
    Nothing
[Returns]       :
    int cutoff_diff(char c1, char c2);	// c1이 c2보다 크면 1, 작으면 -1, 같으면 0 리턴
==================================================================*/
int
cutoff_diff(char c1, char c2)
{
	int diff = c1 - c2; // 두 문자의 차이 연산
	if (diff < 0) // 차이가 음수일 경우 
		diff = -1; // -1로 처리
	else if (diff > 0) // 자이가 양수일 경우
		diff = 1; // 1로 처리
	return diff; //값 리턴
}

/*===============================================================
[Function Name] : int strcmp_p(char *dst, char *src)
[Description]   :
    - 두 스트링을 가르키는 포인터를 받아, 두 스트링의 차이를 구한다.
    - 두 스트링의 차이는 ASCII 코드를 기준으로 한다.
[Input]         :
    char *dst;	// 비교할 첫번째 스트링
    char *src;	// 비교할 두번째 스트링
[Output]        :
    int strcmp_p();	// 두 스트링의 차이를 리턴
    			// dst가 클 경우 1, 작을 경우 -1, 같으면 0 리턴
[Call By]       :
    Main()
[Calls]         :
    Nothiong
[Given]         :
    Nothing
[Returns]       :
    Nothing
==================================================================*/
int
strcmp_p(char *dst, char *src)
{
	while((*dst) && (*src)) // 두 문자 모두 널 문자가 아닐 동안
	{
		if (*dst != *src) // 문자가 다르면
			return (cutoff_diff(*dst, *src)); // 차이 리턴
		dst++; // 같으면 한 글자씩 이동
		src++;
	}
	return (cutoff_diff(*dst, *src)); // 둘 중 하나라도 널 문자이면, 차이 리턴
}

/*===============================================================
[Function Name] : int strcmp_a(char dst[], char src[])
[Description]   :
    - 두 스트링을 저장하는 배열을 받아, 두 스트링의 차이를 구한다.
    - 두 스트링의 차이는 ASCII 코드를 기준으로 한다.
[Input]         :
    char dst[];	// 비교할 첫번째 스트링
    char src[];	// 비교할 두번째 스트링
[Output]        :
    int strcmp_a();	// 두 스트링의 차이를 리턴
    			// dst가 클 경우 1, 작을 경우 -1, 같으면 0 리턴
[Call By]       :
    Main()
[Calls]         :
    Nothing
[Given]         :
    Nothing
[Returns]       :
    Nothing
==================================================================*/
int
strcmp_a(char dst[], char src[])
{
	int i;
	for (i = 0; dst[i] && src[i]; i++) // 두 문자(dst[i], src[i]) 모두 널 문자가 아닐 동안 루프
	{
		if (dst[i] != src[i]) // 두 문자가 다르면
			return (cutoff_diff(dst[i], src[i])); // 차이 리턴
	}
	return (cutoff_diff(dst[i], src[i])); // 둘 중 하나라도 널 문자이면, 차이 리턴
}

main()
{
	int		len1, len2;
	char	str1[20], str2[20];

	len1 = strlen_p("Hello");
	len2 = strlen_a("Hello");
	printf("strlen: p=%d, a=%d\n", len1, len2);

	strcpy_p(str1, "Hello");
	strcpy_a(str2, "Hello");
	printf("strcpy: p=%s, a=%s\n", str1, str2);

	strcat_p(str1, ", World!");
	strcat_a(str2, ", World!");
	printf("strcat: p=%s, a=%s\n", str1, str2);

	printf("과제 2 출력 : \n");
	printf("strcmp: hello and hollo : %d\n", strcmp("hello", "hollo"));
	printf("strcmp: hello and hello : %d\n", strcmp("hello", "hello"));
	printf("strcmp: hello and h : %d\n", strcmp("hello", "h"));
	printf("strcmp_p: hello and hollo : %d\n", strcmp_p("hello", "hollo"));
	printf("strcmp_p: hello and hello : %d\n", strcmp_p("hello", "hello"));
	printf("strcmp_p: hello and h : %d\n", strcmp_p("hello", "h"));
	printf("strcmp_a: hello and hollo : %d\n", strcmp_a("hello", "hollo"));
	printf("strcmp_a: hello and hello : %d\n", strcmp_a("hello", "hello"));
	printf("strcmp_a: hello and h : %d\n", strcmp_a("hello", "h"));
}
