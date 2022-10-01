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
            free(temp);
        }
        //head = NULL;


    }

    /* ************************************************* */

    void load(const char *fname)
    {
    }

    /* ************************************************* */

    void print()
    {
        Node *cur;
        if(head == NULL) {
            printf("List is empty \n");
        } else {
            cur = head;
            while(cur != NULL) {
                printf("N Mec: %d \n", cur->reg.nmec);
            }
        }
    }

    /* ************************************************* */

    void insert(uint32_t nmec, const char *name)
    {
        Node *cur;
        Node *prev;
        Node *n = new Node();
        n->reg.nmec = nmec;
        n->reg.name = name;
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
        
        if(head->reg.nmec == nmec) {
            return head->reg.name;
        } else {
            cur = head;
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
                free(cur);
            } else {
                printf("Student does not belong to the list\n");
            }
        }
    }

    /* ************************************************* */

}
