#include <stdio.h>
#include "semlib.h"
 
// 세마포어 초기화 
int
semInit(key_t key)
{
    int     semid;

    if ((semid = semget(key, 1, 0600 | IPC_CREAT)) < 0)  { // 세마포어 초기화
		perror("semget"); // 에러 처리
        return -1;
    }

    return semid;
}
 
// 세마포어 값 설정
int
semInitValue(int semid, int value)
{
    union semun { // 세마포어 값 넣는 공용체 선언
            int     val;
    } semun;

    semun.val = value; // 공용체에 값 넣기
    if (semctl(semid, 0, SETVAL, semun) < 0)  { // 해당 세마포어에 값 설정
		perror("semctl");
        return -1;
    }
 
    return semid;
}
 
// 세마포어 대기
int
semWait(int semid)
{
    struct sembuf   semcmd;
 
    semcmd.sem_num = 0; // 첫번째 세마포어
    semcmd.sem_op = -1; // 값을 1 감소
    semcmd.sem_flg = SEM_UNDO; // 프로세스가 종료되면 세마포어 설정을 원래대로 되돌림
    if (semop(semid, &semcmd, 1) < 0)  { // 위 설정대로 semid 세마포어에 operation 실행
		perror("semop");
        return -1;
    }
 
    return 0;
}
 
// 세마포어 시도 (wait해야되면 그냥 대기 안함)
int
semTryWait(int semid)
{
    struct sembuf   semcmd;
 
    semcmd.sem_num = 0; // 첫번째 세마포어
    semcmd.sem_op = -1; // 값을 1 감소
    semcmd.sem_flg = IPC_NOWAIT | SEM_UNDO; // 대기해야하면 대기하지 않음 
    if (semop(semid, &semcmd, 1) < 0)  { // 위 설정대로 semid 세마포어에 operation 실행
		perror("semop");
        return -1;
    }
 
    return 0;
}

// 세아포어 값 증가
int
semPost(int semid)
{
    struct sembuf   semcmd;
 
    semcmd.sem_num = 0; // 첫번째 세마포어
    semcmd.sem_op = 1; // 값을 1 증가
    semcmd.sem_flg = SEM_UNDO; // 프로세스가 종료되면 세아포어 설정을 원래대로 되돌림
    if (semop(semid, &semcmd, 1) < 0)  { // 위 설정대로 semid 세아포어에 operation 실행
		perror("semop");
        return -1;
    }
 
    return 0;
}

// 세마포어 값 가져오기
int
semGetValue(int semid)
{
    union semun {
            int     val;
    } dummy;
 
    return semctl(semid, 0, GETVAL, dummy); //GETVAL 로 semctl 실행해 리턴값을 바로 리턴
}
 
// 세마포어 삭제
int
semDestroy(int semid)
{
    union semun {
            int     val;
    } dummy;
 
    if (semctl(semid, 0, IPC_RMID, dummy) < 0)  { // IPC_RMID 로 semctl 실행해 세마포어 제거
		perror("semctl");
        return -1;
    }
    close(semid); // 세마포어 닫기
 
    return 0;
}
