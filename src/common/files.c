#include "files.h"

void * handle_client_request(void * network_data)
{
  // retrieve data in an appropriate way
  NetworkData client_data = *((NetworkData*) network_data);

  printf("Entering handler for port %d\n", ntohs(client_data.private_addr.sin_port));

  char filename[PROTOCOL_BUFFER_SIZE];

  // retrieve filename and client info
  sockaddr_in csin;
  socklen_t csin_size = sizeof(csin);
  if(get_filename(client_data.private_socket, &csin, &csin_size, filename) < 0)
    return NULL;

  printf("%d : Requested filename : %s\n",
                  ntohs(client_data.private_addr.sin_port),
                  filename);

  /* Create appropriate data buffers and mutices then call data_load and data_send in two separate threads */
  // TODO Here

  // create shared data
  char buffer[BUFFER_SIZE];
  pthread_mutex_t mutices[NB_CHUNKS] = {PTHREAD_MUTEX_INITIALIZER};
  // init_mutices(mutices, NB_CHUNKS);

  pthread_t loader_thread;
  pthread_t sender_thread;

  // set data and create corresponding threads
  // loader
  DataLoad_Data load_data;
  load_data.buffer = buffer;
  load_data.mutices = mutices;
  load_data.filename = filename;

  //sender
  DataSend_Data send_data;
  send_data.buffer = buffer;
  send_data.mutices = mutices;
  send_data.sin = client_data.private_addr;
  send_data.sin_size = client_data.len;
  send_data.csin = csin;
  send_data.csin_size = csin_size;
  send_data.sock = client_data.private_socket;

  // launch threads
  pthread_create(&loader_thread, NULL, data_load, (void*)&load_data);
  pthread_create(&sender_thread, NULL, data_send, (void*)&send_data);

  // wait here for our sender thread to complete
  pthread_join(sender_thread, NULL);

  return NULL;
}

void init_mutices(pthread_mutex_t * mutices, int size)
{
  int i;
  for(i=0; i<size; i++)
  {
    // *mutices = PTHREAD_MUTEX_INITIALIZER;
    // mutices++;
  }
}

int get_filename(SOCKET sock, sockaddr_in * sin, socklen_t * sin_size, char * buffer)
{
  int len = recvfrom(sock, buffer, PROTOCOL_BUFFER_SIZE, 0, (sockaddr*)sin, sin_size);
  if(len < 0)
  {
    printf("Error receiving requested filename from client\n");
    return -1;
  }
  return 0;
}
