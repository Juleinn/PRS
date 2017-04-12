#include "data_send.h"

void * data_send(void * data)
{
  // retrieve the address, buffer of the data to sender
  DataSend_Data send_data   = *((DataSend_Data*) data);
  sockaddr_in sin           = send_data.sin;
  socklen_t sin_size        = send_data.sin_size;
  sockaddr_in csin          = send_data.csin;
  socklen_t csin_size       = send_data.csin_size;
  SOCKET sock               = send_data.sock;
  char* data_buffer         = send_data.buffer;
  pthread_mutex_t * mutices = send_data.mutices;

  int seq_id=0;
  while(1)
  {
    pthread_mutex_lock(&(mutices[chunk_position(seq_id)]));
    printf("locked : %d\n", chunk_position(seq_id));


    sleep(1); // temporise

    pthread_mutex_unlock(&(mutices[chunk_position(seq_id)]));
    printf("unlocked : %d\n", chunk_position(seq_id));
    seq_id++;
  }


  /* Start of debugging and test section */
  // debugging buffer
  // char debug_buffer[CHUNK_SIZE+6]="aaaaaaaaaaaaaaaahhhhhhhhhhhhhhhhaaaahahahahah";
  //
  // int seqs;
  // for(seqs = 0; seqs < 500; seqs++)
  // {
  //   sprintf(debug_buffer, "%06d", seqs);
  //   // leave some random stuff in debug buffer
  //   sendto(sock, debug_buffer, sizeof(debug_buffer), 0, (sockaddr*)&csin, csin_size);
  //   // wait for ack
  //   int len = recvfrom(sock, debug_buffer, sizeof(debug_buffer), 0, (sockaddr*)&csin, &csin_size);
  //   printf("debug_buffer : %s \n", debug_buffer);
  // }

  return NULL;
}
