#ifndef DATA_SEND_H
#define DATA_SEND_H

#include "net.h"
#include <pthread.h>

struct DataSend_Data{
  // shared data : buffer & mutices
  char *            buffer;
  pthread_mutex_t * mutices;

  // server side address
  sockaddr_in       sin;
  socklen_t         sin_size;

  // client side address
  sockaddr_in       csin;
  socklen_t         csin_size;

  // private socket opened for communication
  SOCKET            sock;
};
typedef struct DataSend_Data DataSend_Data;

void * data_send(void * data);

#endif
