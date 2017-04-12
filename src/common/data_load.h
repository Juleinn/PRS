#ifndef DATA_LOAD_H
#define DATA_LOAD_H

#include "net.h"
#include <pthread.h>

struct DataLoad_Data{
  // shared data
  char *            buffer;
  pthread_mutex_t * mutices;

  // filename
  char *            filename;
};
typedef struct DataLoad_Data DataLoad_Data;

void * data_load(void * data);

#endif
