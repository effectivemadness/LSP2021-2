#include <stdio.h>
#include <pthread.h>
#include "semlib2.h"


int
sem_init(sem_t *sem, int pshared, int value) // 세마포어 초기화
{
	if (pshared)  { // 지원되지 않는 기능
		fprintf(stderr, "Function not supported\n");
		return -1;
	}

	if (pthread_mutex_init(&sem->mutex, NULL) < 0) // 세마포어 안 mutex 초기화
		return -1;

	if (pthread_cond_init(&sem->cond, NULL) < 0) // 세마포어 안 condition variable 초기화
		return -1;

	sem->sval = value; // 값 설정

	return 0;
}

int
sem_wait(sem_t *sem) // 세마포어 대기
{
	if (pthread_mutex_lock(&sem->mutex) < 0) // 현재 mutex로 잠그기
		return -1;

	if (sem->sval == 0)  { // 값이 0 이면 -> 사용중이면(sval 접근)
		while (sem->sval == 0)  { // 값이 0인 동안
			if (pthread_cond_wait(&sem->cond, &sem->mutex) < 0)  { // 세마포어 post 대기
				if (pthread_mutex_unlock(&sem->mutex) < 0) // cond_wait에서 에러 : lock 했던것 풀고
					return -1;
				return -1;
			}
		}
		sem->sval--; // 값 감소
	}
	else  { // 0이 아니면 그냥 감소
		sem->sval--;
	}

	if (pthread_mutex_unlock(&sem->mutex) < 0) // 세마포어 lock 풀기
		return -1;

	return 0;
}

int
sem_trywait(sem_t *sem) // 세마포어 값 0 이 아니면 감소, 0이면 그냥 리턴
{
	if (pthread_mutex_lock(&sem->mutex) < 0) // 세마포어 mutex lock
		return -1;

	if (sem->sval == 0)  { // 값이 0 이면 
		if (pthread_mutex_unlock(&sem->mutex) < 0) // unlock
			return -1;
		return -1; // 리턴
	}
	else  { // 값이 0 이 아니면
		sem->sval--; // 값 감소
	}

	if (pthread_mutex_unlock(&sem->mutex) < 0) // 세마포어 mutex unlock
		return -1;

	return 0;
}

int
sem_post(sem_t *sem) // 세마포어 post
{
	if (pthread_mutex_lock(&sem->mutex) < 0) // mutex lock 하기
		return -1;

	if (sem->sval == 0)  { // 0이면 대기중인 다른 애들 있을수도 있음 - signal 주기
		if (pthread_cond_signal(&sem->cond) < 0)  { // condition variable 에 signal 주기
			if (pthread_mutex_unlock(&sem->mutex) < 0) // 에러나면 unlock 하고 리턴
				return -1;
			return -1;
		}
	}

	sem->sval++; // 값 증가

	if (pthread_mutex_unlock(&sem->mutex) < 0) // mutex unlock
		return -1;

	return 0;
}

int
sem_getvalue(sem_t *sem, int *sval) // 세마포어 값 가져오기
{
	*sval = sem->sval; // 값 가져오기

	return 0;
}

int
sem_destroy(sem_t *sem) // 세마포어 제거
{
	if (pthread_mutex_destroy(&sem->mutex) < 0) // 뮤텍스 제거
		return -1;

	if (pthread_cond_destroy(&sem->cond) < 0) // condition variable 제거
		return -1;

	return 0;
}

