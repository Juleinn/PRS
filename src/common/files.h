/* Vincent Renotte - Anton Claes
 PRS - created 10/04/2017

 Contains file transfer handling functions
*/

#ifndef FILES_H_
#define FILES_H_

#include <pthread.h>

#include "net.h"
#include "data_send.h"
#include "data_load.h"

/* Structure for packing client data into a void* for further transmission to handler
(supports threading) */
struct NetworkData{
  SOCKET        private_socket;
  sockaddr_in   private_addr;
  socklen_t     len;
};
typedef struct NetworkData NetworkData;

/* Handles one client on a socket with specified port. Connection data is packed into a network_data
structure for later multithreading implementations */
void * handle_client_request(void * network_data);

/*First client message should be filename, this functions will retrieve it */
int get_filename(SOCKET sock, sockaddr_in * sin, socklen_t * sin_size, char * buffer);

/* Initialize an array of mutices to their default values */
void init_mutices(pthread_mutex_t * mutices, int size);


#define PROTOCOL_BUFFER_SIZE 128 // at most 128 bytes of data to receive

#endif
