#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <errno.h>
#include <stdint.h>

#include "delays.h"
#include "thread.h"
#include "dbc.h"

#define SIZE 5

static pthread_mutex_t accessCR = PTHREAD_MUTEX_INITIALIZER;

int old = 0;

void *thread_main(void * arg) {
    
    for(int i = 1; i <= 10; i++) {
        mutex_lock(&accessCR);
        old +=1;
        mutex_unlock(&accessCR);
        usleep(2000);
        
    }
    printf("%3d\n", old);   
    

    return NULL;

}

int main(void) {
    pthread_t my_thread[SIZE];

    //mutex_init(&accessCR, NULL);
    for(int i = 0; i < SIZE; i++) {
        thread_create(&my_thread[i], NULL, &thread_main, NULL);
        
        thread_join(my_thread[i], NULL);
    }

    return 0;
}