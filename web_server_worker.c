//
//  web_server_worker.c
//  NEXT_WS_Sample
//
//  Created by 김창규 on 2014. 11. 21..
//  Copyright (c) 2014년 김창규. All rights reserved.
//

#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include "web_server_worker.h"
#include "event_queue.h"
#include "web_server_boss.h"


#define REQ_BUF_SIZE 1024
#define PATH_BUF_SIZE 256
#define FILE_BUF_SIZE 1024

//todo Thread Pool(Thread Recycling)
//todo Pipelining

int active_thread_cnt = 0;

char* str_comp(char* stream, char* sub_stream);
char* sep_path(char* stream, char* path_buf);
int thread_init();
int thread_release();
int thread_start(int fd);

int OK(int fd);
int BAD_REQUEST(int fd);
int NOT_FOUND(int fd);
int INTERNAL_SERVER_ERROR(int fd);

void* web_service(void *arg)
{
    printf("new Thread Started\n");
    char req_buf[REQ_BUF_SIZE];
    int sock=*((int*)arg);
    recv(sock, req_buf, REQ_BUF_SIZE, 0);
    char* next = str_comp(req_buf,"GET");
    if(next == NULL)
    {
        BAD_REQUEST(sock);
        goto fin;
    }
    
    char path_buf[PATH_BUF_SIZE];
    next = sep_path(next, path_buf);
    
    if(*path_buf != '/')
    {
        BAD_REQUEST(sock);
        goto fin;
    }
    
    char* path = path_buf;
    path++;
    if(*path == '\0')
    {
        path = "index.html";
    }
    
    int res_fd = open(path, O_RDONLY);
    if(res_fd == -1)
    {
        int error = errno;
        if(error == ENOENT)
        {
            NOT_FOUND(sock);
            goto fin;
        }
        INTERNAL_SERVER_ERROR(sock);
        goto fin;
    }
    
    OK(sock);
    int nsize;
    char file_buf[FILE_BUF_SIZE];
    
    while((nsize = read(res_fd, file_buf, FILE_BUF_SIZE))>0)
    {
        send(sock,file_buf,nsize,0);
    }
    
fin:
    close(sock);
    thread_release();
    conn_delete(sock);
    free((int*)arg);
    return NULL;
}

char* str_comp(char* stream, char* sub_stream)
{
    while(*sub_stream != '\0')
    {
        if(*stream != *sub_stream)
            return NULL;
        stream++;
        sub_stream++;
    }
    return stream;
}

char* sep_path(char* stream, char* path_buf)
{
    if(*stream == ' ') stream++;
    while(*stream != ' ')
    {
        *path_buf = *stream;
        stream++;
        path_buf++;
    }
    *path_buf = '\0';
    return stream;
}

int str_cnt(char* arr)
{
    int cnt = 0;
    while(*arr != '\0')
    {
        arr++;;
        cnt++;
    }
    return cnt;
}

int send_msg (int fd, char* msg){return send(fd, msg, str_cnt(msg),0);}
int OK(int fd){return send_msg(fd,"HTTP/1.1 200 OK\r\n\r\n");}
int BAD_REQUEST(int fd){return send_msg(fd,"HTTP/1.1 400 Bad Request\r\n\r\n");}
int NOT_FOUND(int fd){return send_msg(fd,"HTTP/1.1 404 Not Found\r\n\r\n");}
int INTERNAL_SERVER_ERROR(int fd){return send_msg(fd,"HTTP/1.1 500 Internal Server Error\r\n\r\n");}

int thread_release()
{
    active_thread_cnt--;
    while(!thread_start(next_event()));
    return 0;
}

int thread_start(int fd)
{
    if(fd<0 && active_thread_cnt>MAX_WORKER)
    {
        return 0;
    }
    
    pthread_t p;
    int* sock = malloc(sizeof(int));
    *sock = fd;
    int tid=pthread_create(&p, NULL, web_service, sock);
    pthread_detach(p);
    return tid;
}

