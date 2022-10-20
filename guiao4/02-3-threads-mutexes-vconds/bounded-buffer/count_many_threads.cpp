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
    mutex_lock(&accessCR);
    for(int i = 1; i <= 10; i++) {
        old +=1;
        usleep(50000);
    }   
    printf("%3d\n", old);
    mutex_unlock(&accessCR);

    return NULL;

}

int main(void) {
    pthread_t my_thread[SIZE];

    int st;

    
    for(int i = 0; i < SIZE; i++) {
        st = pthread_create(&my_thread[i], NULL, &thread_main, NULL);
        if(st) {
            perror(strerror(st));
            exit(1);
        }

        st = pthread_join(my_thread[i], NULL);
        if(st) {
            perror(strerror(st));
            exit(1);
        }
    }
    
    
    mutex_init(&accessCR, NULL);
    
     
    

    return 0;
}