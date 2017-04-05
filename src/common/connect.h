/* Vincent Renotte - Anton Claes
 PRS - created 5/04/2017

 Contains connection function for TCP implementation over UDP
*/

#ifndef CONNECT_H_
#define CONNECT_H_

#include "net.h"

#define CONNECT_BUFFER_SIZE 128 // 128 bytes for SYN - SYN-ACK - ACK

int udp_accept(SOCKET sock, sockaddr_in *sin, socklen_t sin_size);

#endif
