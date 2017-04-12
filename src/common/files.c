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

  return NULL;
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
