//
//  socket_linked_list.c
//  NEXT_WS_Sample
//
//  Created by 김창규 on 2014. 11. 22..
//  Copyright (c) 2014년 김창규. All rights reserved.
//

#include <stdlib.h>

struct linked_node{
    int fd;
    struct linked_node* next;
};

struct linked_node *s_head, *s_tail;


int sock_list_init()
{
    s_head = malloc(sizeof(struct linked_node));
    s_tail = s_head;
    if(s_head<0)
        return 1;
    return 0;
}

int is_sock_list_empty()
{
    return s_head == s_tail;
}

int add_sock_last(int fd)
{
    struct linked_node* t = malloc(sizeof(struct linked_node));
    t->fd = fd;
    s_tail->next = t;
    s_tail = t;
    
    return 0;
}

int del_scoket(int fd)
{
    struct linked_node *t, *prev;
    
    t=s_head;
    while(t!=s_tail){
        prev=t;
        t=t->next;
        if(t->fd == fd){
            prev->next=t->next;
            if(t==s_tail)
                s_tail=prev;
            free(t);
            return 0;
        }
    }
    return 1;
}


int get_and_del_scoket()
{
    if(s_head == s_tail)
        return -1;
    
    struct linked_node* rem = s_head->next;
    int ret = rem->fd;
    s_head->next = rem->next;
    if(rem == s_tail)
        s_tail = s_head;
    free(rem);
    return ret;
}

int sock_list_finalize()
{
    while(get_and_del_scoket());
    free(s_head);
    return 0;
}