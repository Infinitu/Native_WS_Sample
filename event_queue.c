//
//  event_queue.c
//  NEXT_WS_Sample
//
//  Created by 김창규 on 2014. 11. 22..
//  Copyright (c) 2014년 김창규. All rights reserved.
//

#include <stdlib.h>
#include <pthread.h>
#include "web_server_worker.h"

//todo socket_linked_list와 함께 추상화.



pthread_mutex_t  mutex = PTHREAD_MUTEX_INITIALIZER;

struct socket_event_queue{
    int socket_fd;
    struct socket_event_queue *next;
};

struct socket_event_queue *head, *tail;

int socket_event_queue_init()
{
    head = malloc(sizeof(struct socket_event_queue));
    tail = head;
    return 0;
}

int empty()
{
    return head == tail;
}

int enqueue(int fd)
{
    struct socket_event_queue* t = malloc(sizeof(struct socket_event_queue));
    t->socket_fd = fd;
    tail->next = t;
    tail = t;
    
    return 0;
}

int dequeue()
{
    if(head == tail)
        return -1;
    
    struct socket_event_queue* rem = head->next;
    int ret = rem->socket_fd;
    head->next = rem->next;
    if(rem == tail)
        tail = head;
    free(rem);
    return ret;
}

int socket_event_queue_finalize()
{
    while(dequeue());
    free(head);
    pthread_mutex_destroy(&mutex);
    return 0;
}

int new_event(int fd)
{
    pthread_mutex_lock(&mutex);
    if(empty()){
        if(thread_start(fd))
        {
            pthread_mutex_unlock(&mutex);
            return 0;
        }
    }
    
    enqueue(fd);
    pthread_mutex_unlock(&mutex);
    return 0;
}
int next_event()
{
    pthread_mutex_lock(&mutex);
    int ret = dequeue();
    pthread_mutex_unlock(&mutex);
    return ret;
}


