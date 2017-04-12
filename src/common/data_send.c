#include "data_send.h"

void * data_send(void * data)
{
  // retrieve the address, buffer of the data to sender
  DataSend_Data send_data = *((DataSend_Data*) data);



  printf("Hello from sender\n");
  return NULL;
}
