/* File net.h
 Anton CLAES (C) 2017
 contains general std lib includes for both client and server */
#ifndef NET_H_
#define NET_H_

typedef int SOCKET;

typedef struct sockaddr_in sockaddr_in;
typedef struct sockaddr sockaddr;

#define SOCKET_ERROR -1

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <errno.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <sys/time.h>
#include <arpa/inet.h>

#endif
