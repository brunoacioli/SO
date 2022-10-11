#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <errno.h>
#include <sys/sem.h>
#include <stdint.h>
#include "process.h"
#include "utils.h"

#define N 6


static int* pcounter = NULL;
static int semid;

void increment() {
    int old = *pcounter;
    random_int(0,10000);
    *pcounter = old + 1;

}

int main(void) {
    static short unsigned int access = 0;
    int ppid[N];
    //int fork;

    semid = pshmget(IPC_PRIVATE, sizeof(int), 0600 | IPC_CREAT | IPC_EXCL);

    pcounter = (int*)pshmat(semid, NULL, 0);
    *pcounter = 0;

    

    for(int i = 0; i < N; i++) {
        ppid[i] = pfork();
        if(ppid[i] == 0) {
        psem_up(semid, access);
        increment();
        psem_down(semid, access);
        
        exit(0);
        }

    }
    
    for(int i = 0; i < N; i++) {
        pwaitpid(ppid[i], NULL, 0);
    }
    printf("%d\n", *pcounter);

    
}
