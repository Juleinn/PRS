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

#define CHUNK_SIZE 1024
#define NB_CHUNKS  10
#define BUFFER_SIZE CHUNK_SIZE * NB_CHUNKS

#define SEQ_HEADER_SIZE 6 // 6 bytes for the sequence if then the proper payload

#define chunk_position(seq_id) (seq_id % NB_CHUNKS)

#endif
