//
//  socket_linked_list.h
//  NEXT_WS_Sample
//
//  Created by 김창규 on 2014. 11. 23..
//  Copyright (c) 2014년 김창규. All rights reserved.
//


int sock_list_init();

int is_sock_list_empty();

int add_sock_last(int fd);

int del_scoket(int fd);

int get_and_del_scoket();

int sock_list_finalize();