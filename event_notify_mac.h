//
//  event_notify_mac.h
//  NEXT_WS_Sample
//
//  Created by 김창규 on 2014. 11. 23..
//  Copyright (c) 2014년 김창규. All rights reserved.
//


int epoll_create(int x);

int add_read_event(int fd, int socket, void* event);

int delete_read_event(int fd, int socket, void* event);

void* create_event_array(int size);

void* create_event();

void* get_event_idx(void* events, int idx);

int get_events(int fd, void* events, int size);

int get_event_socket(void* event);

int is_read(void* event);

int is_eof(void* event);


