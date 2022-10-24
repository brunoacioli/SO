#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <errno.h>
#include <stdint.h>

#include "delays.h"
#include "thread.h"
#include "dbc.h"

void *thread_main(void * arg) {
    int max = 10;

    for(int i = 1; i <= max; i++) {
        printf("%3d\n",i);
        usleep(1000000);
    }
    
    return NULL;
}

int main(void) {
    pthread_t my_thread;

    thread_create(&my_thread, NULL, &thread_main, NULL);
    
    thread_join(my_thread, NULL);

    return 0;
}