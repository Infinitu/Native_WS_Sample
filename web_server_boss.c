//
//  web_server_boss.c
//  NEXT_WS_Sample
//
//  Created by 김창규 on 2014. 11. 21..
//  Copyright (c) 2014년 김창규. All rights reserved.
//
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>


#include "web_server_boss.h"
#include "socket_linked_list.h"
#include "event_notify_mac.h"
#include "event_queue.h"

#define MAX_EPOLL_EVENT 1024
#define MAX_BACKLOG 100
#define ERROR 1;

pthread_mutex_t sock_list_mutex = PTHREAD_MUTEX_INITIALIZER;
int serv_sock=-1;
int epoll_fd=-1;
void *ev, *events;


void finalize_various();
int watch_loop();
int conn_add(int fd);
int recv_msg(int fd);

int run_webserver(int port)
{
    int ret;
    
    
    struct sockaddr_in addr;
    
    serv_sock = socket(PF_INET, SOCK_STREAM, 0);
    sock_list_init();
    socket_event_queue_init();
    
    if(serv_sock == -1)
    {
        perror("socket");
        return ERROR;
    }
    
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);
    
    ret = bind(serv_sock, (struct sockaddr*) &addr, sizeof(addr));
    
    if(ret == -1)
    {
        perror("bind");
        finalize_various();
        return ERROR;
    }
    
    ret = listen(serv_sock, MAX_BACKLOG);
    if(ret<0)
    {
        perror("listen");
        finalize_various();
        return ERROR;
    }
    
    printf("Web Server Started.\n");
    
    epoll_fd = epoll_create(10);
    ev = create_event();
    events = create_event_array(MAX_EPOLL_EVENT);
    
    if(epoll_fd == -1)
    {
        perror("epoll_fd");
        finalize_various();
        return ERROR;
    }

    if (add_read_event(epoll_fd, serv_sock, events) == -1){
        perror("add event");
        finalize_various();
        return ERROR;
    }
    
    ret = watch_loop();
    
    finalize_various();
    return ret;
}


int watch_loop() {
    int nev, i;
    struct sockaddr_storage addr;
    socklen_t socklen = sizeof(addr);
    int fd;
    
    while(1) {
        nev = get_events(epoll_fd, events, MAX_EPOLL_EVENT);
        if (nev < 1){
            perror("event polling");
            return 1;
        }
        
        for (i=0; i<nev; i++) {
            if (is_eof(get_event_idx(events, i))) {
                printf("disconnect\n");
                fd = get_event_socket(get_event_idx(events, i));
                if (delete_read_event(epoll_fd, fd, ev) == -1){
                    perror("delete disconnected fd");
                    return 1;
                }
                conn_delete(fd);
            }
            else if (get_event_socket(get_event_idx(events, i)) == serv_sock) {
                fd = accept(serv_sock, (struct sockaddr *)&addr,&socklen);
                if (fd == -1){
                    perror("accept");
                    return 1;
                }
                if (conn_add(fd) == 0) {
                    if (add_read_event(epoll_fd, fd, ev) == -1)
                    {
                        perror("add new conn");
                        return 1;
                    }
                } else {
                    printf("connection refused\n");
                    close(fd);
                }
                
            }
            else if (is_read(get_event_idx(events, i))) {
                recv_msg(get_event_socket(get_event_idx(events, i)));
            }
        }
    }
}


int conn_add(int fd)
{
    printf("new Connection.\n");
    pthread_mutex_lock(&sock_list_mutex);
    add_sock_last(fd);
    pthread_mutex_unlock(&sock_list_mutex);
    return 0;
}

int recv_msg(int fd)
{
    return new_event(fd);
}

void conn_delete(int fd)
{
    pthread_mutex_lock(&sock_list_mutex);
    del_scoket(fd);
    pthread_mutex_unlock(&sock_list_mutex);
}




void finalize_various(){
    if(serv_sock>0)close(serv_sock);
    if(epoll_fd>0)close(epoll_fd);
    if(ev!=NULL)free(ev);
    if(events!=NULL)free(events);
    
    int fd = get_and_del_scoket();
    while(fd > 0){
        close(fd);
        fd = get_and_del_scoket();
    }
    sock_list_finalize();
    socket_event_queue_finalize();
    return;
}
