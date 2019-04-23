#include <stdlib.h>
#include "fsm.h"
#include <unistd.h>
#include <stdlib.h>
#include <sys/time.h>
#include <stdio.h>

#define timeout_follower 150
#define timeout_candidate 150
#define timeout_leader 150

int
timeval_less (const struct timeval* a, const struct timeval* b)
{
  return (a->tv_sec == b->tv_sec)? (a->tv_usec < b->tv_usec) :
    (a->tv_sec < b->tv_sec);
}

void
timeval_add (struct timeval* res,
             const struct timeval* a, const struct timeval* b)
{
  res->tv_sec = a->tv_sec + b->tv_sec;
  res->tv_usec = a->tv_usec + b->tv_usec;
  if (res->tv_usec >= 1000000) {
    res->tv_sec += res->tv_usec / 1000000;
    res->tv_usec = res->tv_usec % 1000000;
  }
}

void
timeval_sub (struct timeval* res,
             const struct timeval* a, const struct timeval* b)
{
  res->tv_sec = a->tv_sec - b->tv_sec;
  res->tv_usec = a->tv_usec - b->tv_usec;
  if (res->tv_usec < 0) {
    res->tv_sec --;
    res->tv_usec += 1000000;
  }
}

void
timeval_random (struct timeval* res, int ms)
{
  res->tv_sec = 0;
  res->tv_usec = rand() % (ms * 1000);
}

enum { MSG_VOTE, MSG_REQVOTE, MSG_APPENDENTRIES };

struct message_t {
  int src_id;
  int type;
};
typedef struct message_t message_t;

static message_t* peekMessage(void) {
  static message_t msg = { -1, -1 } ;
  return &msg;
}

enum { FOLLOWER, CANDIDATE, LEADER };

struct fsm_raft_t {
  fsm_t fsm;
  int term;
  int votes;

  int nnodes;
  struct timeval next;
};
typedef struct fsm_raft_t fsm_raft_t;

static int receivedAppendEntries (fsm_t* this) {
  message_t* msg = peekMessage();
  return msg->type == MSG_APPENDENTRIES;
}

static int receivedReqForVote (fsm_t* this) {
  message_t* msg = peekMessage();
  return msg->type == MSG_REQVOTE;
}

static int receivedVote (fsm_t* this) {
  message_t* msg = peekMessage();
  return msg->type == MSG_VOTE;
}

static int timeoutFollower (fsm_t* this) {
  fsm_raft_t* raft = (fsm_raft_t*) this;
  struct timeval now;
  gettimeofday (&now, NULL);
  return timeval_less (&raft->next, &now);
}

static int timeoutCandidate (fsm_t* this) {
  fsm_raft_t* raft = (fsm_raft_t*) this;
  struct timeval now;
  gettimeofday (&now, NULL);
  return timeval_less (&raft->next, &now);
}

static int timeoutLeader (fsm_t* this) {
  fsm_raft_t* raft = (fsm_raft_t*) this;
  struct timeval now;
  gettimeofday (&now, NULL);
  return timeval_less (&raft->next, &now);
}

static int majorityOfVotes (fsm_t* this) {
  fsm_raft_t* raft = (fsm_raft_t*) this;
  return raft->votes > (raft->nnodes - raft->votes);
}

static void replyAppendEntries(fsm_t *this)
{
  //reply append entries
  i = 0;
  for (i; nnodes; i++)
  {
    if (msg->append == 1)
    {
      log_num[i] = log_num[i] + 1;
    }
    else if (msg->append == 0)
    {
      log_num[i] = Log_num[i] - 1;
    }
  }
  //timeout
  fsm_raft_t *raft = (fsm_raft_t *)this;
  struct timeval now;
  gettimeofday (&now, NULL);
  timeval_random(&raft->next, timeout_follower);//añade 150 y parte aleatoria de hasta 150
  timeval_add(&raft->next, &raft->next, &now);
  timeval_add(&raft->next, &raft->next, timeout_follower);
  printf("replyAppendEntries\n");
}

static void sendVote (fsm_t* this) {
  fsm_raft_t* raft = (fsm_raft_t*) this;
  raft->term = term_recv(); //actualizar mi term, hay que implementar term_recv()
  //mensaje
  message_t* msg;
  msg->src_id; //hay que establecer el id en el mensaje con el mio, habrá que guardar el id del nodo en la struct fsm_raft_t
  msg->type = MSG_VOTE;
  //udp_server_send_Vote(&msg); falta por implementar (en funcion de como se reciben los msg)

  printf("sendVote\n");
}

static void sendReqForVote (fsm_t* this) {
  fsm_raft_t* raft = (fsm_raft_t*) this;
  //candidatura
  raft->term++;
  raft->votes=1;
  //mensaje
  message_t* msg;
  msg->src_id; //hay que establecer el id del mensaje con el que tengo
  msg->type = MSG_REQVOTE;
  //udp_server_send_ReqForVote(&msg); falta por implementar (en funcion de como se reciben los msg)

  struct timeval now;
  gettimeofday (&now, NULL);
  timeval_add(&raft->next, timeout_candidate , &now);//mirar tiempos 
  printf("sendReqForVote\n");
}

static void sendAppendEntries(fsm_t *this)
{
  //send appent entries
  i = 0;
  for (i; nnodes; i++)
  {
    send.logEntry(term, log_num[0], array_id[i]);
  }
  //timeout
  fsm_raft_t *raft = (fsm_raft_t *)this;
  struct timeval now;
  gettimeofday (&now, NULL);
  timeval_add(&raft->next, timeout_leader , &now);//mirar tiempos
  printf("sendAppendEntries\n");
}

static void incVotes (fsm_t* this) {
  fsm_raft_t* raft = (fsm_raft_t*) this;
  printf("incVotes\n");
  raft->votes++;
}

static void resetFollowerTimeout (fsm_t* this) {
  
  printf("resetFollowerTimeout\n");
}

static void resetCandidateTimeout (fsm_t* this) {
  printf("resetCandidateTimeout\n");
}

static void resetLeaderTimeout (fsm_t* this) {
  printf("resetLeaderTimeout\n");
}

static void service_request (fsm_t* this) {
  printf("serviceRequest\n");
}


fsm_t* fsm_new_leader (int id)
{
  static fsm_trans_t tt[] = {
     { FOLLOWER,  receivedAppendEntries, FOLLOWER,  replyAppendEntries },
    { FOLLOWER,  receivedReqForVote,    FOLLOWER,  sendVote },
    { FOLLOWER,  timeoutFollower,       CANDIDATE, sendReqForVote },
     { CANDIDATE, receivedVote,          CANDIDATE, incVotes },
     { CANDIDATE, timeoutCandidate,      CANDIDATE, sendReqForVote },
     { CANDIDATE, receivedReqForVote,    FOLLOWER,  sendVote },
     { CANDIDATE, majorityOfVotes,       LEADER,    resetLeaderTimeout },
     { LEADER,    timeoutLeader,         LEADER,    sendAppendEntries },
     { LEADER,    receivedAppendEntries, FOLLOWER,  resetFollowerTimeout },
     {-1, NULL, -1, NULL },
    };
  fsm_raft_t* raft = (fsm_raft_t*) malloc (sizeof (fsm_raft_t));
  fsm_t* this = (fsm_t*) raft;
  fsm_init (this, tt);
  raft->term = 0;
  raft->votes = 0;
  raft->nnodes = 0;
  gettimeofday (&raft->next, NULL);
  return this;
}


fsm_t*
fsm_log_replication (int id)
{
  static fsm_trans_t tt[] = {
     { FOLLOWER,  majorityOfVotes,       LEADER,    elections_winned },
     { FOLLOWER,  service_request,       FOLLOWER,  send_request_leader },
     { FOLLOWER,  append_recv_ok,        FOLLOWER,  add_log },
     { FOLLOWER,  append_recv_fail,      FOLLOWER,  send_fail },
     { LEADER,    service_request,       LEADER,    new_log },
     { LEADER,    recv_follower_fail,    LEADER,    upgrade_idc_less },
     { LEADER,    recv_follower_ok,      LEADER,    upgrade_idc_add },
     { LEADER,    check_majority,        LEADER,    commit },
     { LEADER,    new_leader,            FOLLOWER,  leave_leadership },
     {-1, NULL, -1, NULL },
    };
  fsm_raft_t* raft = (fsm_raft_t*) malloc (sizeof (fsm_raft_t));
  fsm_t* this = (fsm_t*) raft;
  fsm_init (this, tt);
  raft->term = 0;
  raft->votes = 0;
  raft->nnodes = 0;
  gettimeofday (&raft->next, NULL);
  return this;
}

int
main (int argc, char* argv[]) {
  int id = (argc > 1)? atoi(argv[1]) : 0;
  fsm_t* fsm_leader = fsm_new_leader (id);
  while (1) {
    
    fsm_fire (fsm_new_leader);
    fsm_fire (fsm_log_replication);
    sleep (1);
  }
}
