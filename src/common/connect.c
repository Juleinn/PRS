#include "connect.h"

int udp_accept(SOCKET sock, sockaddr_in *sin, socklen_t * sin_size)
{
  printf("waiting for incomming connection\n");
  char connect_buffer[CONNECT_BUFFER_SIZE];
  // wait for incomming connection request
  int len = recvfrom(sock, connect_buffer, CONNECT_BUFFER_SIZE, 0, (sockaddr*) sin, sin_size);

  if(len < 0)
  {
    printf("Connection error\n");
    return -1;
  }

  if(memcmp("SYN", connect_buffer, strlen("SYN")) == 0)
  {
    // received valid SYN signal
    // pick a new port and bind

  }
  else
  {
    // received invalid SYN signal : discard
    printf("Received corrupt \"SYN\" message. Discarding\n");
    return -1;
  }

  return 0;
}

int pick_port()
{
  // pick port
  return (rand() % 8999) + 1000;
}

int free_bind(SOCKET csock, sockaddr_in* csin, socklen_t csin_size)
{
  do{
    csin->sin_family = AF_INET;
    csin->sin_addr.s_addr = INADDR_ANY;
    csin->sin_port = htons(pick_port());
  }while(bind(csock, (sockaddr*)csin, csin_size) < 0);

  return 0;
}
