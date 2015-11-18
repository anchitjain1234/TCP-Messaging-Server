//
// Created by anchit on 17/11/15.
//

#ifndef SELECT_SERVER_SELECT_SERVER_H
#define SELECT_SERVER_SELECT_SERVER_H

#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <unistd.h>
#include <strings.h>
#include <sys/select.h>
#include <stdbool.h>
#include <string.h>

#define LISTENQ 15
#define MAXLINE 800
#define MAX_CLIENTS 500
#define MAX_GROUPS 100
#endif //SELECT_SERVER_SELECT_SERVER_H
