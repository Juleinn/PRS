#include "connect.h"

int udp_accept(SOCKET sock, sockaddr_in *sin, socklen_t sin_size)
{
  char connect_buffer[CONNECT_BUFFER_SIZE]
  // wait for incomming connection request
  int len = recvfrom(sock, connect_buffer, CONNECT_BUFFER_SIZE(sockaddr*) sin, sin_size);

  return 0;
}
