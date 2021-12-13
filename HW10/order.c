#include <stdio.h>

int	main()
{
	int		a = 0x12345678; // 16진수 12345678 변수 선언
	char	*p = (char *)&a; // 변수 주소를 char 포인터 변수 p에 저장

#if 1
	printf("Address %p: %#x\n", p, *p); p++; // p 위치에서 1바이트 출력후 1바이트 이동
	printf("Address %p: %#x\n", p, *p); p++; // p 위치에서 1바이트 출력후 1바이트 이동
	printf("Address %p: %#x\n", p, *p); p++; // p 위치에서 1바이트 출력후 1바이트 이동
	printf("Address %p: %#x\n", p, *p); p++; // p 위치에서 1바이트 출력후 1바이트 이동
#else
	printf("Address %p: %#x\n", p, *p++);
	printf("Address %p: %#x\n", p, *p++);
	printf("Address %p: %#x\n", p, *p++);
	printf("Address %p: %#x\n", p, *p++);
#endif
}
