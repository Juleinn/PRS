#include <stdio.h>
#include <stdlib.h>
#include "net.h"
#include "connect.h"
#include "files.h"

int main(int argc, char** argv)
{
  printf("Starting server\n");

  SOCKET sock = socket(AF_INET, SOCK_DGRAM, 0); // wait for incomming
  if(sock < 0)
  {
    fprintf(stderr, "%s\n", strerror(errno));
    return EXIT_FAILURE;
  }

  sockaddr_in sin;
  sin.sin_family = AF_INET;
  sin.sin_addr.s_addr = INADDR_ANY;
  sin.sin_port = htons(2000);
  socklen_t sin_size = sizeof(sin);

  if(bind(sock, (sockaddr*)&sin, sin_size) < 0)
  {
    fprintf(stderr, "%s\n", strerror(errno));
    return EXIT_FAILURE;
  }

  sockaddr_in csin;
  socklen_t csin_size = sizeof(csin);
  SOCKET csock = udp_accept(sock, &sin, &sin_size, &csin, &csin_size);

  NetworkData cdata;
  cdata.private_addr = csin;
  cdata.private_socket = csock;
  cdata.len = csin_size;

  handle_client_request((void*)&cdata);

  while(1)
  {

  }

  close(sock);
}
