/* Copyright 2015 Anchit Jain */

#ifndef SELECT_SERVER_H_
#define SELECT_SERVER_H_

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
#include <arpa/inet.h>

#define LISTENQ 15
#define MAXLINE 800
#define MAX_CLIENTS 500
#define MAX_GROUPS 100
#endif  // SELECT_SERVER_H_
