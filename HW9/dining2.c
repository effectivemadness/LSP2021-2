#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <pthread.h>
#include <sys/time.h>

#define	NUM_MEN		5 // 철학자 수
#define	NLOOPS		5 // 생각 - 식사 루프 수

#define	LEFT(i)		((i+NUM_MEN-1)%NUM_MEN) // i의 왼쪽 인덱스
#define	RIGHT(i)	((i+1)%NUM_MEN) // i 의 오른쪽 인덱스

enum  { THINKING, EATING, HUNGRY }; // 생각 식사 배고픔 enum 선언 

sem_t	Philosopher[NUM_MEN]; // 각 철학자 세마포어 배열 
sem_t	Mutex; // state 공유메모리 접근 mutex 
int		State[NUM_MEN]; // 각 철학자 상태 배열

// thread Sleep 하는 함수
void
ThreadUsleep(int usecs)
{
	pthread_cond_t		cond; // condition variable 선언
	pthread_mutex_t		mutex; // mutex 선언
	struct timespec		ts; // 시간 변수들 선언
	struct timeval		tv;

	if (pthread_cond_init(&cond, NULL) < 0)  { // condition variable 초기화
		perror("pthread_cond_init"); // 에러처리
		pthread_exit(NULL);
	}
	if (pthread_mutex_init(&mutex, NULL) < 0)  { // mutex 초기화
		perror("pthread_mutex_init"); // 에러처리
		pthread_exit(NULL);
	}

	gettimeofday(&tv, NULL); // 현재 시간 가져오기
	ts.tv_sec = tv.tv_sec + usecs/1000000; // 초 단위 더하기
	ts.tv_nsec = (tv.tv_usec + (usecs%1000000)) * 1000; // 나노초 단위 더하기
	if (ts.tv_nsec >= 1000000000)  {
		ts.tv_nsec -= 1000000000;
		ts.tv_sec++;
	}

	if (pthread_mutex_lock(&mutex) < 0)  { // mutex lock 설정
		perror("pthread_mutex_lock");
		pthread_exit(NULL);
	}
	if (pthread_cond_timedwait(&cond, &mutex, &ts) < 0)  { // ts에 설정된 시간에 도달하기 전까지 현재 쓰레드 block
			//cond_signal, cond_broadcast 로 condition varialbe 호출되면 unblock
		perror("pthread_cond_timedwait");
		pthread_exit(NULL);
	}

	if (pthread_cond_destroy(&cond) < 0)  { // condition variable 제거
		perror("pthread_cond_destroy");
		pthread_exit(NULL);
	}
	if (pthread_mutex_destroy(&mutex) < 0)  { // mutex variable 제거
		perror("pthread_mutex_destroy");
		pthread_exit(NULL);
	}
}

void
Thinking(int id) // 철학자 생각 함수
{
	printf("Philosopher%d: Thinking.....\n", id);
	ThreadUsleep((rand()%200)*10000); // 랜덤시간동안 sleep
	printf("Philosopher%d: Want to eat.....\n", id);
}

void
Eating(int id) // 철학자 식사 함수
{
	printf("Philosopher%d: Eating.....\n", id);
	ThreadUsleep((rand()%100)*10000); // 랜덤시간동안 sleep
}

void
Test(int id) // 먹을 수 있는지 확인
{
	if (State[id] == HUNGRY && State[LEFT(id)] != EATING 
			&& State[RIGHT(id)] != EATING)  { // 현재 내가 배고프고 양 옆이 모두 식사중이 아니면 나는 먹기 가능
		State[id] = EATING;

		if (sem_post(&Philosopher[id]) < 0)  { // 현재 철학자 먹을 수 있음 post
			perror("sem_post");
			pthread_exit(NULL);
		}
	}
}

void
PickUp(int id)
{
	if (sem_wait(&Mutex) < 0)  { // state 배열 접근 mutex 세마포어 -> 사용중이 아니면 대기
		perror("sem_wait");
		pthread_exit(NULL);
	}

	State[id] = HUNGRY; // 내 상태 배고픔으로 설정 

	Test(id); // 먹을 수 있는지 확인 & 가능한 상태까지 대기후 현재 상태 EATING으로 설정

	if (sem_post(&Mutex) < 0)  { // state 배열 접근 중지 post
		perror("sem_post");
		pthread_exit(NULL);
	}

 	if (sem_wait(&Philosopher[id]) < 0)  { // 현재 철학자 먹을 수 있는지 대기
		perror("sem_wait");
		pthread_exit(NULL);
	}
}

void
PutDown(int id)
{
	if (sem_wait(&Mutex) < 0)  { // state 배열 접근 mutex 대기
		perror("sem_wait");
		pthread_exit(NULL);
	}

	State[id] = THINKING; // 현재 철학자 상태 생각중으로 바꾸기

	Test(LEFT(id)); // 왼쪽 철학자 식사 가능한지 확인
	Test(RIGHT(id)); // 오른쪽 철학자 식사 가능한지 확인

	if (sem_post(&Mutex) < 0)  { // state 배열 접근 중지 post
		perror("sem_post");
		pthread_exit(NULL);
	}
}

void // 철학자 쓰레드 함수
DiningPhilosopher(int *pId)
{
	int		i;
	int		id = *pId; // 현재 철학자 번호 

	for (i = 0 ; i < NLOOPS ; i++)  { // NLOOPS동안 생각 - 식사 반복

		Thinking(id); // 생각

		PickUp(id); // 젓가락 들기 - 양옆 확인후 현재 철학자 먹기 가능 표시

		Eating(id); // 먹기

		PutDown(id); // 젓가락 내려두기 - 양옆 테스트
	}

	printf("Philosopher%d: thinking & eating %d times.....\n", id, i);

	pthread_exit(NULL); // 쓰레드 종료
}

int	main()
{
	pthread_t	tid[NUM_MEN]; // 쓰레드 id 배열 
	int			i, id[NUM_MEN]; // 철학자 번호 배열

	srand(0x8888); // 랜덤 seed

	for (i = 0 ; i < NUM_MEN ; i++)  { // 철학자 수만큼 루프
		if (sem_init(&Philosopher[i], 0, 0) < 0)  { // 각 철학자 먹기 가능한지 표시하는 세마포어 초기화
			perror("sem_init");
			exit(1);
		}
		id[i] = i;
	}
	if (sem_init(&Mutex, 0, 1) < 0)  { // state 배열 접근 mutex 세마포어
		perror("sem_init");
		exit(1);
	}

	for (i = 0 ; i < NUM_MEN ; i++)  { // 철학자 수만큼 루프
		if (pthread_create(&tid[i], NULL, (void *)DiningPhilosopher, 
				(void *)&id[i]) < 0)  { // 각 쓰레드 생성
			perror("pthread_create");
			exit(1);
		}
	}

	for (i = 0 ; i < NUM_MEN ; i++)  { // 철학자 수만큼 루프 
		if (pthread_join(tid[i], NULL) < 0)  { // 각 쓰레드 종료 대기
			perror("pthread_join");
			exit(1);
		}
	}

	for (i = 0 ; i < NUM_MEN ; i++)  {
		if (sem_destroy(&Philosopher[i]) < 0)  { // 철학자 식사가능 세마포어 제거
			perror("sem_destroy");
		}
	}

	if (sem_destroy(&Mutex) < 0)  { // state 배열 접근 mutex 세마포어 제거
		perror("sem_destroy");
		exit(1);
	}

}