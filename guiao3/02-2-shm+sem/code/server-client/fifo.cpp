#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <errno.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <stdint.h>
#include <string.h>

#include "fifo.h"
#include "delays.h"
#include "process.h"

namespace fifo
{
    #define FIFOSZ  5

    struct BUFFER {
        uint32_t id;
        char* text;
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
        BUFFER slot[FIFOSZ];
    };

    int fifoFreeId = -1;
    int fifoPendingId = -1;
    int bufferId = -1;
    int semBufferId = -1;
    

    FIFO *fifoFree = NULL;
    FIFO *fifoPending = NULL;

    BUFFER *buffer = NULL;


    uint32_t iiBuffer;
    uint32_t riBuffer;
    uint32_t cntBuffer;


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
        fifoFreeId = pshmget(IPC_PRIVATE, sizeof(FIFO), 0600 | IPC_CREAT | IPC_EXCL);
        
        /*  attach shared memory to process addressing space */
        fifoFree = (FIFO*)pshmat(fifoFreeId, NULL, 0);

        // Creation of FIFO of Pending requests

        /* create the shared memory */
        fifoPendingId = pshmget(IPC_PRIVATE, sizeof(FIFO), 0600 | IPC_CREAT | IPC_EXCL);

        /*  attach shared memory to process addressing space */
        fifoPending = (FIFO*)pshmat(fifoPendingId, NULL, 0);


        // Creation of BUFFER

        /* create the shared memory */
        bufferId = pshmget(IPC_PRIVATE, FIFOSZ * sizeof(BUFFER), 0600 | IPC_CREAT | IPC_EXCL);

        /*  attach shared memory to process addressing space */
        buffer = (BUFFER*)pshmat(bufferId, NULL, 0);

        for(int i = 0; i < FIFOSZ; i++) {

            fifoFree->slot[i].id = 99999;
            fifoFree->slot[i].text = "Init";
            fifoFree->slot[i].len = 0;
            fifoFree->slot[i].num_digits = 0;
            fifoFree->slot[i].num_letters = 0;

            fifoPending->slot[i].id = 99999;
            fifoPending->slot[i].text = "Init";
            fifoPending->slot[i].len = 0;
            fifoPending->slot[i].num_digits = 0;
            fifoPending->slot[i].num_letters = 0;

            buffer[i].id = 99999;
            buffer[i].text = "Init";
            buffer[i].len = 0;
            buffer[i].num_digits = 0;
            buffer[i].num_letters = 0;

        }

        // Create access, full, empty and waiting response semaphores

        fifoFree->semid = psemget(IPC_PRIVATE, 3, 0600 | IPC_CREAT | IPC_EXCL);
        fifoPending->semid = psemget(IPC_PRIVATE, 3, 0600 | IPC_CREAT | IPC_EXCL);
        semBufferId = psemget(IPC_PRIVATE, 3, 0600 | IPC_CREAT | IPC_CREAT | IPC_EXCL);

        for (int i = 0; i < FIFOSZ; i++) {
            up(fifoFree->semid, NSLOTS);
            up(fifoPending->semid, NSLOTS);
            up(semBufferId, NSLOTS);
        }

        up(fifoFree->semid, ACCESS);
        up(fifoPending->semid, ACCESS);
        up(semBufferId, ACCESS);

        //up(fifoFree->semid, RESPONSE);
        //up(fifoPending->semid, RESPONSE);


    }

    void destroy() {

        /* detach shared memory from process addressing space */
        pshmdt(fifoFree);
        pshmdt(fifoPending);

        /* destroy the shared memory */
        pshmctl(fifoFreeId, IPC_RMID, NULL);
        pshmctl(fifoPendingId, IPC_RMID, NULL);
    }

    void putRequestData(uint32_t id, char* data) {
        down(semBufferId, NSLOTS);
        down(semBufferId, ACCESS);

        buffer[iiBuffer].id = id;
        buffer[iiBuffer].text = strdup(data);
 
        iiBuffer = (iiBuffer + 1) % 5;
        cntBuffer++;

        up(semBufferId, ACCESS);
        up(semBufferId, NSLOTS);

        //terminar depois
    }

    uint32_t getFreeBuffer() {

        down(fifoFree->semid, NITEMS);
        down(fifoFree->semid, ACCESS);

        uint32_t id = fifoFree->slot[fifoFree->ri].id;
        fifoFree->ri = (fifoFree->ri + 1) % FIFOSZ;
        fifoFree->cnt--;


        up(fifoFree->semid, ACCESS);
        up(fifoFree->semid, NSLOTS);

        return id;
    }

}