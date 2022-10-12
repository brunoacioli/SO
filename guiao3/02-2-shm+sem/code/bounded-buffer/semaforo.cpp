#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <errno.h>
#include <sys/sem.h>
#include <stdint.h>
#include "process.h"
#include "utils.h"

#define N 5
#define ACCESS 0


static int* pcounter = NULL;
static int counterId;
static int semid;

void increment() {
    psem_down(semid, ACCESS);
    int old = *pcounter;
    random_int(0,10000);
    *pcounter = old + 1;
    psem_up(semid, ACCESS);

}

int main(void) {
    //int ppid[N];
    int fork;

    //Criaçao do semaforo
    semid = psemget(IPC_PRIVATE, 1, 0600 | IPC_CREAT | IPC_EXCL);
    psem_up(semid,ACCESS);

    //Criaçao do counter
    counterId = pshmget(IPC_PRIVATE, sizeof(int), 0600 | IPC_CREAT | IPC_EXCL);
    pcounter = (int*)pshmat(counterId, NULL, 0);
    *pcounter = 0;

    
    

    

    for(int i = 0; i < N; i++) {
        fork = pfork();
        if(fork == 0) {
            increment();
            exit(0);
        }

    }
    
    for(int i = 0; i < N; i++) {
        pwait(NULL);
    }
    printf("%d\n", *pcounter);

    
}
