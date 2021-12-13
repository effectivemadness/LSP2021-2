#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <pthread.h>
#include <sys/time.h>

#define	NUM_MEN		5 // 철학자 수
#define	NLOOPS		5 // 각 철학자마다 생각-식사 루프 수

sem_t	ChopStick[NUM_MEN]; // 젓가락 수 = 철학자 수 = 세마포어 수

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

void // 철학자 생각하기
Thinking(int id)
{
	printf("Philosopher%d: Thinking.....\n", id);
	ThreadUsleep((rand()%200)*10000); // 랜덤시간동안 sleep
	printf("Philosopher%d: Want to eat.....\n", id); // 이제 먹고 싶다
}

void // 철학자 밥먹기
Eating(int id)
{
	printf("Philosopher%d: Eating.....\n", id); // 먹는중
	ThreadUsleep((rand()%100)*10000); // 랜덤시간동안 sleep
}

void // 각 철학자 쓰레드 함수
DiningPhilosopher(int *pId)
{
	int		i; // for loop iter
	int		id = *pId; // 철학자 일련번호

	for (i = 0 ; i < NLOOPS ; i++)  { // 생각 - 식사 루프

		Thinking(id); // 철학자 생각

		if (sem_wait(&ChopStick[id]) < 0)  { // 자신 오른쪽 젓가락 사용 가능한지 확인 & 못쓰면 대기
			perror("sem_wait");
			pthread_exit(NULL);
		}
		if (sem_wait(&ChopStick[(id+1) % NUM_MEN]) < 0)  { // 자신 왼쪽 젓가락 사용 가능한지 확인 & 못쓰면 대기
			perror("sem_wait");
			pthread_exit(NULL);
		}

		Eating(id); // 이 지점 -> 양쪽 젓가락 전부 들고 있음 -> 식사

		if (sem_post(&ChopStick[id]) < 0)  { // 자신 오른쪽 젓가락 사용 중단 post
			perror("sem_post");
			pthread_exit(NULL);
		}
		if (sem_post(&ChopStick[(id+1) % NUM_MEN]) < 0)  { // 자신 왼쪽 젓가락 사용 중단 post
			perror("sem_post");
			pthread_exit(NULL);
		}
	}

	printf("Philosopher%d: thinking & eating %d times.....\n", id, i); // 다 먹음 표시

	pthread_exit(NULL); // 쓰레드 종료
}

int	main()
{
	pthread_t	tid[NUM_MEN]; // 각 철학자 쓰레드 배열
	int			i, id[NUM_MEN]; // 각 철학자 일련번호 배열

	srand(0x8888); // 랜덤 Seed

	for (i = 0 ; i < NUM_MEN ; i++)  { // 철학자 수 만큼 루프
		if (sem_init(&ChopStick[i], 0, 1) < 0)  { // 젓가락 세마포어 초기화
			perror("sem_init");
			exit(1);
		}
		id[i] = i; // 일련번호 배열 초기화
	}

	for (i = 0 ; i < NUM_MEN ; i++)  { // 철학자 수만큼 루프
		if (pthread_create(&tid[i], NULL, (void *)DiningPhilosopher, 
				(void *)&id[i]) < 0)  { // 철학자 쓰레드 실행
			perror("pthread_create");
			exit(1);
		}
	}

	for (i = 0 ; i < NUM_MEN ; i++)  { // 철학자 수만큼 루프
		if (pthread_join(tid[i], NULL) < 0)  { // 철학자 쓰레드 종료 및 join 대기
			perror("pthread_join");
			exit(1);
		}
	}

	for (i = 0 ; i < NUM_MEN ; i++)  { // 철학자 수 = 젓가락 개수만큼 루프
		if (sem_destroy(&ChopStick[i]) < 0)  { // 젓가락 세마포어 제거
			perror("sem_destroy"); 
		}
	}
}
