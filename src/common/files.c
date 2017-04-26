#include "files.h"

void packet_loss(double * cwnd, Congestion_mode * mode, int * flightSize)
{
  *cwnd /= 2;// /= 2 ;// slow start & C.A
  *mode = CONGESTION_AV; //nevermind we already might be
  *flightSize = 0; // we must assume we lost everything

  // just to reset to slow start in case packet loss happen in a row
  if((int) *cwnd == 0)
  {
    *cwnd = 1;
    *mode = SLOWSTART;
  }
}

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

  // open file
  FILE* f = fopen(filename, "r");
  if(f==NULL)
  {
    fprintf(stderr, "Unable to open file \"%s\"\n", filename);
    return NULL;
  }

  /// -------------------------------------------------------------------
  /// Here begins the transmission section
  /// -------------------------------------------------------------------
  // some RFC defined variables
  int lastSeqAcked = -1;
  double cwnd = 1.0; // Will be cast to int later as it is handy to be able to add fractions to it
  Congestion_mode cmode = SLOWSTART;
  int RTT = 3000.0;  // very large RTT as it will be meseared upon first round trip.
  #define RTT_offset 50 // add RTT offset to the actuel rtt to get timeout value
  int flightSize = 0;
  // get the last sequence's if to know when to stop
  int filesize = get_filesize(f);
  int lastSeq = ((filesize / CHUNK_SIZE) + (filesize % CHUNK_SIZE != 0 ? 1 : 0)) - 1;
  // payload & header buffer
  char buffer[CHUNK_SIZE + SEQ_HEADER_SIZE];
  // seqs sending times
  #define SERVER_WND 10000
  SeqMeta seq_metas[SERVER_WND];

  // enter proper transmission loop
  int done = 0;

  // debugging variables
  int debugLoop = 60;
  long long int startms = getMillis();

  FILE* acks_logs = fopen("acks_f_time.txt", "w");
  FILE* cwnd_logs = fopen("cwnd_f_time.txt", "w");
  FILE* rtt_logs = fopen("rtt_f_time.txt", "w");
  FILE* seqid_logs = fopen("seqid.txt", "w");

  while(lastSeqAcked != lastSeq)
  {
    for(;flightSize < (int) cwnd && lastSeqAcked + flightSize < lastSeq; flightSize++)
    {
      int seq_id = lastSeqAcked + flightSize + 1;     // compute the sequence's id
      sprintf(buffer, "%06d", seq_id + 1);            // print it to the header
      fseek(f, seq_id * CHUNK_SIZE, SEEK_SET);        // seek position in file
      int payload = fread(buffer + SEQ_HEADER_SIZE, 1, CHUNK_SIZE, f);  // read from file

      RTT_chrono(seq_id, START, seq_metas, SERVER_WND); // start RTT chronometer

      // send data
      sendto(client_data.private_socket, buffer, SEQ_HEADER_SIZE + payload, 0,
              (sockaddr*)&csin, csin_size);
    }

    int len = wait_ack(client_data.private_socket, buffer, SEQ_HEADER_SIZE + CHUNK_SIZE,
      &csin, &csin_size, RTT + RTT_offset);

    if(len < 0) // transmission error : assuming packet loss
    {
      printf("Timed out\n");
      packet_loss(&cwnd, &cmode, &flightSize);
    }
    else  // transmission successfull
    {
      int ack_id = get_ack_seqid(buffer);
      updateRTT(&RTT, ack_id, seq_metas, SERVER_WND);

      if(ack_id > lastSeqAcked)
      {
        flightSize -= ack_id - lastSeqAcked;
        lastSeqAcked = ack_id;

        // discriminate appropriate congestion mode
        if(cmode == SLOWSTART)
          cwnd++;
        else
          cwnd += 1/cwnd;
        fprintf(cwnd_logs, "%lld;%lf\n", getMillis() - startms, cwnd);

      }
      else if (ack_id == lastSeqAcked)
      {
        printf("Duplicated : acked %d\n", ack_id);
        packet_loss(&cwnd, &cmode, &flightSize);
      }
    }
  }


  // spam a few 'FIN' messages
  sprintf(buffer, "FIN");
  sendto(client_data.private_socket, buffer, 4, 0, (sockaddr*)&csin, csin_size);
  sendto(client_data.private_socket, buffer, 4, 0, (sockaddr*)&csin, csin_size);
  sendto(client_data.private_socket, buffer, 4, 0, (sockaddr*)&csin, csin_size);
  sendto(client_data.private_socket, buffer, 4, 0, (sockaddr*)&csin, csin_size);
  sendto(client_data.private_socket, buffer, 4, 0, (sockaddr*)&csin, csin_size);
  sendto(client_data.private_socket, buffer, 4, 0, (sockaddr*)&csin, csin_size);


  // close file
  fclose(f);
  fclose(acks_logs);
  fclose(cwnd_logs);
  fclose(rtt_logs);
  fclose(seqid_logs);
  return NULL;
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
    printf("buffer = %s\n", buffer);
    return atoi(buffer+3) - 1;
  }
  else // corrupt format
  {
    printf("warning : Corrupt acknowledgement\n");
    return -1;
  }
}

int wait_ack(SOCKET sock, char * buffer, int size, sockaddr_in * csin, socklen_t * csin_size, int timeout)
{
  udp_rcv_timeout(sock, timeout);
  return recvfrom(sock, buffer, size, 0, (sockaddr*) csin, csin_size);
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
