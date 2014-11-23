//
//  event_notify_mac.c
//  NEXT_WS_Sample
//
//  Created by 김창규 on 2014. 11. 22..
//  Copyright (c) 2014년 김창규. All rights reserved.
//
#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>
#include <stdlib.h>



int epoll_create(int x)
{
    return kqueue();
}

int add_read_event(int fd, int socket, void* event)
{
    if(event==NULL) return 1;
    EV_SET((struct kevent*)event, socket, EVFILT_READ, EV_ADD, 0, 0, NULL);
    return kevent(fd, (struct kevent*)event, 1, NULL, 0, NULL) ;
}

int delete_read_event(int fd, int socket, void* event)
{
    EV_SET((struct kevent*)event, socket, EVFILT_READ, EV_DELETE, 0, 0, NULL);
    return kevent(fd, (struct kevent*)event, 1, NULL, 0, NULL);
}

void* create_event_array(int size)
{
    return malloc(sizeof(struct kevent)*size);
}

void* create_event()
{
    return malloc(sizeof(struct kevent));
}

void* get_event_idx(void* events, int idx)
{
    return events+(idx*sizeof(struct kevent));
}

int get_events(int fd, void* events, int size)
{
    return kevent(fd, NULL, 0, events, size, NULL);
}

int get_event_socket(void* event)
{
    return ((struct kevent*)event)->ident;
}

int is_read(void* event)
{
    return ((struct kevent*)event)->flags & EVFILT_READ;
}

int is_eof(void* event)
{
    return ((struct kevent*)event)->flags & EV_EOF;
}


