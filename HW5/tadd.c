#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

typedef struct tData
{
		int	start;
		int	end;
		int	result;
} tData_t;
/*===============================================================
[Function Name] : void *getSum
[Description]   : 
    - tData 구조체에서 구간 합의 시작점, 종료점을 받아 계산한다.
	- 계산값은 tData 구조체의 result 변수에 저장한다.
[Input]         :
    tData_t // 쓰레드에서 필요한 구간합 시작 및 종료점과 결과값 저장 변수가 있는 구조체
[Output]        :
    tData_t->result // 구한 구간합의 결과
[Call By]       :	        
    Main()
[Calls]         :	        
    Nothiong
[Given]         :
    Nothing
[Returns]       :
    Nothing
==================================================================*/
void	*getSum(void *tin)
{
		int	ret = 0; // 임시 값 저장 변수
		tData_t *data = (tData_t *)tin; // 인자로 받은 포인터를 데이터 구조체 형으로 캐스팅
		for (int i = data->start; i <= data->end; i++)
				ret += i; // 시작 ~ 종료점까지 합
		data->result = ret; // 값을 구조체 result 변수에 저장 
		pthread_exit(NULL);		// 쓰레드 종료
}

/*===============================================================
[Program Name] : tadd
[Description]  : 
    - 1~50, 51~100 의 합을 각 쓰레드로 구해 최종적으로 1~100 의 합을 구하는 프로그램
[Input]        :
    -
[Output]       :
    - 콘솔로 각 쓰레드에서 구한 합을 출력한 후, 두 수의 합을 출력한다.
[Calls]        :	        
    - thread : void *getSum(void *tin) // 각 쓰레드에서 구할 구간 합의 시작 및 종료점, 결과값을 전달
[특기사항]     : 쓰레드와 주고받을 데이터를 tData 구조체를 선언해 사용한다.
==================================================================*/
int	main()
{
		pthread_t	tid1, tid2; // 쓰레드 ID 2개 선언
		tData_t	tin1, tin2; // 쓰레드와 입출력할 변수 선언

		tin1.start = 1; // 시종점 선언
		tin1.end = 50;
		tin2.start = 51;
		tin2.end = 100;

		if (pthread_create(&tid1, NULL, (void *)getSum, (void *)&tin1) < 0) // 첫번째 쓰레드 실행
		{
				perror("pthread_create");
				exit(1);
		}
		if (pthread_create(&tid2, NULL, (void *)getSum, (void *)&tin2) < 0) // 두번째 쓰레드 실행
		{
				perror("pthread_create");
				exit(1);
		}
		
		if (pthread_join(tid1, NULL) > 0) // 첫번째 쓰레드 종료 대기
		{
				perror("pthread_join");
				exit(1);
		}
		if (pthread_join(tid2, NULL) > 0) // 두번째 쓰레드 종료 대기
		{
				perror("pthread_join");
				exit(1);
		}

		printf("1 ~ 50 : %d\n51~100 : %d\n1 ~ 100 : %d\n", tin1.result, tin2.result, tin1.result + tin2.result);
		// 각 쓰레드의 결과를 출력 후 최종 합을 출력
}
