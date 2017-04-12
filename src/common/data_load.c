#include <stdio.h>

#include "data_load.h"


void * data_load(void * data)
{
  //Init
  DataLoad_Data* dataUse = data;
  printf("%s\n", dataUse->filename);

  FILE *fp;
  fp = file = fopen(dataUse->filename, "r");

  while(1)
  {

    //Lock mutex (writing in process...)
    fread(dataUse->buffer[0+CHUNK_SIZE*0], CHUNK_SIZE, 1, fp);
    //Unlock mutex (reading in process)
  }

 return data;
}
