#include "files.h"

/* Switch this to be a big finitie state automata */
double cwnd;                // congestion window
int lastSeqAcked;           // id of the last sequence acked
int seq_id;                 // current id of the sequence being sent
int flightSize;             // number of sequences currently sent but not acked
int RTT;                    // Round trip time
char buffer[SEQ_HEADER_SIZE + CHUNK_SIZE];  // data + header buffer (UDP data)
FILE* f;                    // the source file to read from
int lastSeq;                // last sequence of the file
int sstresh;                // rfc defined variable-> could use a mode instead
int ack_id;                 // id of the current acked sequence
int dupli_counter;          // counter for duplicate acks
#define TIME_BUFFER_SIZE 1000000 // seq_ids are stored on 6 digits -> max 999999
long long int send_millis[TIME_BUFFER_SIZE];  // sending times of seqs for RTT estimation

// setupable factor for optimisation
#ifdef CUSTOM_CWND
  #define DEFAULT_CWND CUSTOM_CWND
#else
  #define DEFAULT_CWND 1
#endif

#ifdef CWND_FAC
  #define CWND_UPONLOSS_FACTOR CWND_FAC
#else
  #define CWND_UPONLOSS_FACTOR .85;
#endif

#ifdef CUS_CHK
  #undef CHUNK_SIZE
  #define CHUNK_SIZE CUS_CHK
#endif

#define ALPHA 0.8
#define BETA 1.8

#ifdef CUS_ALPHA
    #undef ALPHA
    #define ALPHA CUS_ALPHA
#endif

#ifdef CUS_BETA
  #undef BETA
  #define BETA CUS_BETA
#endif


#define update_RTT(){\
  int est_RTT = (getMillis() - send_millis[lastSeqAcked]);\
  RTT = ((float) est_RTT * ALPHA) + ((float) RTT * (1 - ALPHA));\
}\

#define RTO (int) (BETA * (float) RTT)

void * handle_client_request2(SOCKET sock, sockaddr_in sin, socklen_t sin_size,
  sockaddr_in csin, socklen_t csin_size)
{
  printf("BETA = %lf\n", BETA);
  // retrieve filename from transmission
  get_filename(sock, &csin, &csin_size, buffer);
  // then load it
  f = fopen(buffer, "r");
  // get filesize for statistical data measurement
  long long int filesize = get_filesize(f);

  if(f == NULL)
  {
    // fprintf(stderr, "Unable to open file \"%s\"\n", buffer);
    return NULL;
  }

  init();

  // get start time
  long long int startMillis = getMillis();

  // now enter proper finite state automata
  while(lastSeqAcked != lastSeq)
  {
    // send data
    send_seqs(sock, &csin, csin_size);

    // update timeout according to RTT
    udp_rcv_timeout(sock, RTO); // 50 ms timeout


    // wait for any ack
    if(wait_ack(sock, &csin, &csin_size) == ACK_TIMEDOUT)
    {
      timedout();
      printf("Timed out\n");
    }
    else // ACK_OK (assumed not corrupt)
    {
      if(ack_id > lastSeqAcked) // correct ack
      {
        correct_ack();
      }
      else if(ack_id == lastSeqAcked)  // duplicate_ack
      {
        duplicate_ack(sock, &csin, &csin_size);
      }
    }
  }

  printf("End of transmission\n");
  printf("filesize = %lld bytes : %.1fKB : %.1fMB\n", filesize, (float)filesize / 1000.0f, (float) filesize/(1000000.0f));
  int timeMillis = (int) (getMillis() - startMillis);
  printf("Time : %d ms : %.1fs\n", timeMillis, (float) timeMillis / 1000.0f);
  printf("Rate : %.1fKB/s, %.2fMB/s\n", ((float)filesize)/(timeMillis), ((float)filesize/(1000))/((float)timeMillis));

  // end of transmission : send 'FIN'
  sprintf(buffer, "FIN");
  sendto(sock, buffer, 4, 0, (sockaddr*)&csin, csin_size);

  return NULL;
}

// sends given sequences
void send_seqs(SOCKET sock, sockaddr_in * sin, socklen_t sin_size)
{
  // send sequences so that lastSeqAcked + flightSize < lastSeq (stops at end of file)
  // and send the right amount of sequences : flightSize < cwnd
  for(; flightSize < cwnd && lastSeqAcked + flightSize < lastSeq; flightSize++)
  {
    // compute sequence id
    seq_id = lastSeqAcked + flightSize + 1; // seq id starting at 0 -> clients expects them to start at 1
    // write header into buffer
    sprintf(buffer, "%06d", seq_id + 1);// client-side seq_id standart
    // seek file to position & load
    fseek(f, seq_id * CHUNK_SIZE, SEEK_SET);
    int payload = fread(buffer + 6, 1, CHUNK_SIZE, f);

    // send the sequence
    sendto(sock, buffer, SEQ_HEADER_SIZE + payload, 0, (sockaddr*)sin, sin_size);
    // printf("Sent seq %d \n", seq_id);

    // update sending time accordingly
    send_millis[seq_id] = getMillis();
  }
}

// waits for an acknowledgement and handles it
int wait_ack(SOCKET sock, sockaddr_in*sin, socklen_t* sin_size)
{
  int len = recvfrom(sock, buffer, SEQ_HEADER_SIZE + CHUNK_SIZE, 0, (sockaddr*)sin, sin_size);
  if(len < 0) // timedout
  {
    return ACK_TIMEDOUT;
  }
  else
  {
    // check validity
    if(memcmp(buffer, "ACK", 3) == 0)
    {
      ack_id = atoi(buffer + 3) - 1; // client side standart
      return ACK_OK;
    }
    else // corrupt ack
    {
      fprintf(stderr, "Corrupt ack\n");
      return ACK_CORRUPT;
    }
  }
}

void correct_ack()
{
  // update values accordingly
  // update cwnd and flightSize (different if in S.S or C.A)
  flightSize -= (ack_id - lastSeqAcked);

  if(cwnd < sstresh)// slow start
    cwnd++;
  else
    cwnd += 1/cwnd;
  lastSeqAcked = ack_id;

  update_RTT();

  // printf("Acked %d\n", lastSeqAcked);
}

void duplicate_ack(SOCKET sock, sockaddr_in* sin, socklen_t *sin_size)
{
  // printf("Received a duplicate ack\n");
  // handle fast retransmit and fast recovery
  // count the amount of duplicate acks (seems to be 3 like in TCP)
  // we should wait 2 others acks
  dupli_counter++;
  if(dupli_counter == 3)// fast retransmit
  {
    // reset dupli_counter
    dupli_counter = 0;
    fast_retransmit(sock, sin, sin_size);
  }
}

void fast_retransmit(SOCKET sock, sockaddr_in* sin, socklen_t* sin_size)
{
  // printf("Fast retransmitting %d\n", ack_id);
  // send the next sequence and wait for an ack. If it fails, time it out, otherwise continue with same cwnd
  // send single seq
  seq_id = ack_id + 1;
  // write header into buffer
  sprintf(buffer, "%06d", seq_id + 1);// client-side seq_id standart
  // seek file to position & load
  fseek(f, seq_id * CHUNK_SIZE, SEEK_SET);
  int payload = fread (buffer + 6, 1, CHUNK_SIZE, f);

  // send the sequence
  sendto(sock, buffer, SEQ_HEADER_SIZE + payload, 0, (sockaddr*)sin, *sin_size);

  // leave the fast_retransmit, main loop will detect if it has failed
}

// handles timeout
void timedout()
{
  cwnd *= CWND_UPONLOSS_FACTOR;
  flightSize = 0;
  sstresh = (int) cwnd;

  // make sure not to get stuck with cwnd = 0 in case of multiple failures
  if((int) cwnd == 0)
  {
    cwnd = DEFAULT_CWND;
    sstresh = 1000000;// slow start again
  }
}

// initializes the automata for transmission
void init()
{
  cwnd = DEFAULT_CWND;
  lastSeqAcked = -1;// starting sequences at 0
  seq_id = 0;
  flightSize = 0; // currently no sequence sent
  RTT = 50; // large RTT for first transmission as it will be measured later
  sstresh = 1000000; // set it to a large value to leave slow start only upon packet loss
  dupli_counter = 0;

  // compute last sequence
  int filesize = get_filesize(f);
  lastSeq = (filesize / CHUNK_SIZE) + (filesize % CHUNK_SIZE ? 1 : 0) - 1;
}

void packet_loss(double * cwnd, Congestion_mode * mode, int * flightSize)
{
  *cwnd *= CWND_UPONLOSS_FACTOR;// /= 2 ;// slow start & C.A
  *mode = CONGESTION_AV; //nevermind we already might be
  *flightSize = 0; // we must assume we lost everything

  // just to reset to slow start in case packet loss happen in a row
  if((int) *cwnd == 0)
  {
    *cwnd = DEFAULT_CWND;
    *mode = SLOWSTART;
  }
}

void updateRTT(int * RTT, int seq, SeqMeta * meta, int seqs)
{
  int tmpRTT;
  if((tmpRTT = RTT_chrono(seq, STOP, meta, seqs)) != -1)
    *RTT = tmpRTT;
}

int get_ack_seqid(char* buffer)
{
  // expects the ACK to have the format ACKXXXXXX
  if(memcmp(buffer, "ACK", 3) == 0) // correct format
  {
    // printf("buffer = %s\n", buffer);
    return atoi(buffer+3) - 1;
  }
  else // corrupt format
  {
    printf("warning : Corrupt acknowledgement\n");
    return -1;
  }
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

long int get_filesize(FILE* f)
{
	long pos = ftell(f);
	fseek(f, 0, SEEK_END);
	long size = ftell(f);
	fseek(f, pos, SEEK_SET);
	return size;
}


int udp_rcv_timeout(SOCKET sock, int timeout)
{
	struct timeval tv;
	tv.tv_sec = timeout / 1000;
	tv.tv_usec = (timeout%1000) * 1000;
	int retval;
	if((retval = setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char*) &tv, sizeof(tv))) != 0)
		printf("udp_rcv_timeout error\n");
	return retval;
}

int RTT_chrono(int seq, RTT_chronomode mode, SeqMeta * meta, int seqs)
{
  // position of the sequence in the array
  int index = seq % seqs;
  // compute current time value
  struct timeval tv;
  gettimeofday(&tv, NULL);
  if(mode == START)
  {
    meta[index].seq_id = seq;
    meta[index].startMillis = (tv.tv_sec * 1000 + tv.tv_usec / 1000);
    return -1; // no meaningfull time value to return
  }
  else
  {
    if(meta[index].seq_id == seq)
    {
      return (int)(tv.tv_sec * 1000 + tv.tv_usec / 1000) - meta[index].startMillis;
    }
    else
    {
      // seq. ids not corresponding
      return -1;
    }
  }
}

// for performance &packet loss measurment
long long int getMillis()
{
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return (tv.tv_sec * 1000) + (tv.tv_usec / 1000);
}
