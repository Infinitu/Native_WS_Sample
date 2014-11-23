//
//  main.c
//  NEXT_WS_Sample
//
//  Created by 김창규 on 2014. 11. 20..
//  Copyright (c) 2014년 김창규. All rights reserved.
//
#include "web_server_boss.h"
#include <stdio.h>

#define DEF_PORT 8888

int main(int argc, const char * argv[]) {
    
    int port = DEF_PORT;
    
    if(argc>2) {
        printf("Usage : %s <port>\n", argv[0]);
        return 1;
    }
    
    if(argc == 2)
    {
        port = atoi(argv[1]);
    }

    run_webserver(port);

    printf("Good Bye");
    return 0;
}
