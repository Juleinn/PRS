#include <stdio.h>

#include "data_load.h"

void * data_load(void * data)
{
  //Init
  DataLoad_Data* dataUse = data;
  printf("%s\n", dataUse->filename);

  FILE *fp;
  fp = fopen(dataUse->filename, "r");

  int n = 0;
  while(1)
  {
    //Increase position of cursor in circular buffer
    if(n==NB_CHUNKS-1) n=0;
    else n++;

    //Lock mutex (writing in process...)
    if(pthread_mutex_lock(&(dataUse->mutices[n]))!=0)
    {
      perror("pthread_mutex_lock:");
    }
    printf("loader : locked %d\n", n);

    //Read Data in filename and copy it in buffer
    fread(&(dataUse->buffer[CHUNK_SIZE*n]), CHUNK_SIZE, 1, fp);
    printf("Hello from data_load\n");

    //Unlock mutex (allows data_send to use resource)
    if(pthread_mutex_unlock(&(dataUse->mutices[n]))!=0)
    {
      perror("pthread_mutex_unlock:");
    }
    printf("loader : unlocked %d\n", n);
  }

 return NULL;
}
