#include "connect.h"

int udp_accept(SOCKET sock, sockaddr_in *sin, socklen_t * sin_size,
  sockaddr_in* client_addr, socklen_t * client_size)
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
    printf("Received client \"SYN\"\n");
    // send back SYN-ACK:PORT
    // pick random port, bind and ack with port number
    sockaddr_in csin;
    SOCKET csock = socket(AF_INET, SOCK_DGRAM, 0);

    if(csock < 0)
    {
      printf("Error creating socket for new client\n");
      return -1;
    }

    free_bind(csock, &csin, sizeof(csin));

    printf("Switching to free port %d\n", ntohs(csin.sin_port));
    sprintf(connect_buffer, "SYN-ACK%04d", (int) ntohs(csin.sin_port));

    // send SYN-ACK
    len = sendto(sock, connect_buffer, strlen("SYN-ACK0000") + 1, 0, (sockaddr*) sin, *sin_size);

    if(len < 0)
    {
      printf("Connection error at SYN-ACK\n");
      return -1;
    }

    int len = recvfrom(sock, connect_buffer, CONNECT_BUFFER_SIZE, 0, (sockaddr*) sin, sin_size);

    if(len < 0)
    {
      printf("Error receiving ACK from client\n");
      return -1;
    }

    if(memcmp(connect_buffer, "ACK", strlen("ACK")) == 0)
    {
      // return newly created socket and corresponding address
      // copy at the very last moment from csin to client_addr
      *client_addr = csin;
      *client_size = sizeof(csin);
      return csock;
    }
    else
    {
        printf("Received corrupt \"ACK\" message. Discarding\n");
    }

    // wait for incomming ACK from client

    // ack :
    // sprintf(connect_buffer, "SYN-ACK%d")
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
