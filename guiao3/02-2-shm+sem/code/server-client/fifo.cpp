#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <errno.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <stdint.h>
#include <string.h>
#include <cstring>
#include <iostream>

#include "fifo.h"
#include "delays.h"
#include "process.h"

namespace fifo
{
    #define FIFOSZ  5
    #define MAX     129

    struct ITEM
    {
        int semid;
        uint32_t id;
        char text[MAX];
        int len;
        int num_letters;
        int num_digits;
    };

    struct FIFO 
    {
        int semid;
        uint32_t ii;
        uint32_t ri;
        uint32_t cnt;
        uint32_t slot[FIFOSZ];
    };

    struct SharedAll
    {
        FIFO fifos[2];
        ITEM pool[FIFOSZ];
    };

    int sharedDataId = -1;
    

    SharedAll* sharedData = NULL;


    /* index of access, full and empty semaphores */
    #define ACCESS 0
    #define NITEMS 1
    #define NSLOTS 2
    //#define RESPONSE 3

    /* ************************************************* */

    static void down(int semid, unsigned short index)
    {
        struct sembuf op = {index, -1, 0};
        psemop(semid, &op, 1);
    }

    /* ************************************************* */

    static void up(int semid, unsigned short index)
    {
        struct sembuf op = {index, 1, 0};
        psemop(semid, &op, 1);
    }

    void create(void) {
        // Creation of FIFO of free buffers

        /* create the shared memory */
        sharedDataId = pshmget(IPC_PRIVATE, sizeof(SharedAll), 0600 | IPC_CREAT | IPC_EXCL);
        
        /*  attach shared memory to process addressing space */
        sharedData = (SharedAll*)pshmat(sharedDataId, NULL, 0);

        for(int i = 0; i < FIFOSZ; i++) {
            sharedData->fifos[0].slot[i] = i;
            sharedData->fifos[1].slot[i] = 99;

            sharedData->pool[i].id = i;
            strcpy(sharedData->pool[i].text, "Init");
            sharedData->pool[i].len = 0;
            sharedData->pool[i].num_digits = 0;
            sharedData->pool[i].num_letters = 0;
        }

        // Create access, full, empty and waiting response semaphores

        for(int i = 0; i < FIFOSZ; i++) {
            sharedData->pool[i].semid = psemget(IPC_PRIVATE, 1, 0600 | IPC_CREAT | IPC_EXCL);
            up(sharedData->pool[i].semid, ACCESS);
        }

        for(int i = 0; i < 2; i++) {
            sharedData->fifos[i].semid = psemget(IPC_PRIVATE, 1, 0600 | IPC_CREAT | IPC_EXCL);
            for(int j = 0; j < FIFOSZ; j++) {
                up(sharedData->fifos[i].semid, NSLOTS);
            }
            up(sharedData->fifos[i].semid, ACCESS);
        }


    }

    void destroy() {

        /* detach shared memory from process addressing space */
        pshmdt(sharedData);

        /* destroy the shared memory */
        pshmctl(sharedDataId, IPC_RMID, NULL);
    }

    void putRequestData(uint32_t id, char* data) {
    
    }

    uint32_t getFreeBuffer() {

        return 1;
    }

}