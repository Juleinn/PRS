/* Vincent Renotte - Anton Claes
 PRS - created 10/04/2017

 Contains file transfer handling functions
*/

#ifndef FILES_H_
#define FILES_H_

#include <pthread.h>

#include "net.h"

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
void * handle_client_request2(SOCKET sock, sockaddr_in sin, socklen_t sin_size,
  sockaddr_in csin, socklen_t csin_size);

/*First client message should be filename, this functions will retrieve it */
int get_filename(SOCKET sock, sockaddr_in * sin, socklen_t * sin_size, char * buffer);

/* Initialize an array of mutices to their default values */
void init_mutices(pthread_mutex_t * mutices, int size);

/* Allows setting a timeout value for socket, in ms*/
int udp_rcv_timeout(SOCKET sock, int timeout);

long long int getMillis();

enum RTT_chronomode{
  START, STOP
};
typedef enum RTT_chronomode RTT_chronomode;

struct SeqMeta{
  int seq_id;
  long long int startMillis;
};
typedef struct SeqMeta SeqMeta;

enum Congestion_mode{
  SLOWSTART, CONGESTION_AV
};
typedef enum Congestion_mode Congestion_mode;

/* Allows to measure RTT for a given seq_id */
int RTT_chrono(int seq, RTT_chronomode mode, SeqMeta * meta, int seqs);

#define PROTOCOL_BUFFER_SIZE 128 // at most 128 bytes of data to receive

long int get_filesize(FILE* f);

void packet_loss(double * cwnd, Congestion_mode * mode, int * flightSize);

int get_ack_seqid(char* buffer);

void updateRTT(int * RTT, int seq, SeqMeta * meta, int seqs);

void send_seqs(SOCKET sock, sockaddr_in * sin, socklen_t sin_size);

int wait_ack(SOCKET sock, sockaddr_in*sin, socklen_t* sin_size);

void init();

void timedout();

void correct_ack();

void duplicate_ack(SOCKET sock, sockaddr_in* sin, socklen_t* sin_size);

void fast_retransmit(SOCKET sock, sockaddr_in* sin, socklen_t* sin_size);

#define ACK_TIMEDOUT -1
#define ACK_OK 0
#define ACK_CORRUPT 1

#endif
