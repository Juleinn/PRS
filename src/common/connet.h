/* Vincent Renotte - Anton Claes
 PRS - created 5/04/2017

 Contains connection function for TCP implementation over UDP
*/

#ifndef CONNECT_H_
#define CONNECT_H_

#include "net.h"

int udp_accept(SOCKET sock, sockaddr_in *sin, socklen_t sin_size);

#endif
