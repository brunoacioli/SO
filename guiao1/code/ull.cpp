/*
 *
 * \author (2016) Artur Pereira <artur at ua.pt>
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <stdint.h>
#include <iostream>
#include <string.h>
#include <assert.h>

#include "ull.h"

namespace ull
{
    /* ************************************************* */

    /* The information support data structure  */
    struct Register
    {
        uint32_t nmec;       //!< student number
        const char *name;    //!< student name
    };

    /* The linked-list support data structure */
    struct Node 
    {
        Register reg;
        struct Node *next;
    };

    static Node *head = NULL;

    /* ************************************************* */

    void reset()
    {
        Node *temp;

        while(head != NULL) {
            temp = head;
            head = head->next;
            free((void*)temp->reg.name);
            free(temp);
        }
        //head = NULL;

        // tirar dúvidas com o professor segunda


    }

    /* ************************************************* */

    void load(const char *fname)
    {
        FILE* ptr;
        char* token;
        char str[129];
        char name[129];
        char* strPtr = str;
        char* namePtr = name;
        char* etpr;
        
        int i;
        int total = 0;
        uint32_t nmec;
        ptr = fopen(fname,"r");
        

        /*fgets(strPtr, 129, ptr);
        token = strtok(strPtr, ";");
        while(token != NULL){
            printf("%s\n", token);
            token = strtok(NULL, ";");
        }*/
        
        

        if(ptr == NULL) {
            printf("file can't be opened");
            exit(1);
        }

        while(!feof(ptr)) {
            total++;
            i = 0;
            fgets(strPtr,129,ptr);
            //printf("%s\n", strPtr);
            token = strtok(strPtr, ";");
            while(token != NULL) {
                //printf("%s\n", token);
                if(i%2 == 0) {
                    namePtr = token;
                } else {
                    //printf("N MEC: %s\n", token);
                    nmec = strtoul(token, &etpr, 10);
                }
                token = strtok (NULL, ";");
                i++;
            }
            insert(nmec, namePtr);
        }
        printf("Total: %u", total);
        fclose(ptr);
    }

    /* ************************************************* */

    void print()
    {
        int total = 0;
        Node *cur;
        if(head == NULL) {
            printf("List is empty \n");
        } else {
            cur = head;
            while(cur != NULL) {
                total++;
                printf("N Mec - %d  %s \n", cur->reg.nmec, cur->reg.name);
                cur = cur->next;
            }
        }
        printf("Total: %i", total);
    }

    /* ************************************************* */

    void insert(uint32_t nmec, const char *name)
    {
        Node *cur;
        Node *prev;
        Node *n = new Node();
        n->reg.nmec = nmec;
        n->reg.name = strdup(name);
        if (head == NULL) {
            head = n;
            head->next = NULL;
        } else {
            prev = NULL;
            cur = head;
            while(cur != NULL && cur->reg.nmec < nmec) {
                    prev = cur;
                    cur = cur->next;
            }
            if(prev == NULL) {
                n->next = head;
                head = n;
            }
            else {
                n->next = cur;
                prev->next = n; 
            }
         }

    }

    /* ************************************************* */

    const char *query(uint32_t nmec)
    {
        Node *cur;
        cur = head;
        
        if(head->reg.nmec == nmec) {
            return head->reg.name;
        } else {
            while (cur != NULL && cur->reg.nmec != nmec){
                cur = cur->next;
            }
            if(cur->reg.nmec == nmec) {
                return cur->reg.name;
            }
        }

        return NULL;
    }

    /* ************************************************* */

    void remove(uint32_t nmec)
    {
        assert(query(nmec) != NULL);
        Node *cur;
        Node *prev;

        cur = head;

    

        if(head->reg.nmec == nmec) {
            head = head->next;
            free(cur);
        } else {
            while(cur != NULL && cur->reg.nmec != nmec) {
                prev = cur;
                cur = cur->next;
            }
            if(cur->reg.nmec == nmec) {
                prev->next = cur->next;
                free((void*)cur->reg.name);
                free(cur);
            }
        }
    }

    /* ************************************************* */

}
