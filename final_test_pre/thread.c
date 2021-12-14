#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "semlib2.h"
#include <strings.h>

sem_t	MutexSem;
char	Msg[100];

void Producer(void *dummy)
{
	if (sem_init(&MutexSem, 0, 0) < 0)
	{
		perror("sem_init");
		pthread_exit(NULL);
	}
	// 세마포어 생성과 동시에 0으로 초기화 -> 이 쓰레드가 소유권 가짐
	bzero(Msg, sizeof(Msg));
	strcpy(Msg, "hello");
	if(sem_post(&MutexSem) < 0)
	{
		perror("sem_post");
		pthread_exit(NULL);
	}
	pthread_exit(NULL);
}

void Consumer(void *dummy)
{
	if (sem_wait(&MutexSem) < 0)
	{
		perror("sem_wait");
		pthread_exit(NULL);
	}
	printf("%s\n", Msg);
	if(sem_post(&MutexSem) < 0)
	{
		perror("sem_post");
		pthread_exit(NULL);
	}

}

int main()
{
	pthread_t	prod_tid, consum_tid;

	if (pthread_create(&prod_tid, NULL, (void*)Producer, (void*)NULL) < 0){
		perror("pthread_Create");
	}

	if (pthread_create(&consum_tid, NULL, (void*)Consumer, (void*)NULL) < 0){
		perror("pthread_Create");
	}

	if (pthread_join(prod_tid, NULL) < 0)
	{
		perror("pthread_join");
	}
	if (pthread_join(consum_tid, NULL) < 0)
	{
		perror("pthread_join");
	}

	if (sem_destroy(&MutexSem) < 0)
	{
		perror("sem_destroy");
		exit(1);
	}
}
