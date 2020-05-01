#include <stdlib.h>
#include <string.h>
#include "fsm.h"
#include <unistd.h>
#include <stdio.h>
#include "udp_server.h"
#include "interp.h"
#include "timeval_helper.h"

#define MAX_NODES    50
#define MAX_MESSAGES 50
#define MAX_COUNTERS 50

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

enum { MSG_VOTE =          'v',
       MSG_REQVOTE =       'R',
       MSG_APPENDENTRIES = 'A' };

struct message_t {
  int src_id;
  int type;
  int term;
  int prev_term;
  int prev_logidx;
  //const char entry[];
  int leader_commit_logidx;
};
typedef struct message_t message_t;

struct raft_counter_t {
  const char* name;
  int value;
};
typedef struct raft_counter_t raft_counter_t;

enum { FOLLOWER =  'F',
       CANDIDATE = 'C',
       LEADER =    'L' };

struct fsm_raft_t {
  fsm_t fsm;
  int id[MAX_NODES];
  int nnodes;

  int term;
  int votes;

  struct timeval next;

  message_t msg[MAX_MESSAGES];
  int nmsg;

  udp_server_t* me;
  udp_client_t* neighbor[MAX_NODES - 1];

  struct raft_counter_t stats[MAX_COUNTERS];
  int nstats;
};
typedef struct fsm_raft_t fsm_raft_t;

void
message_init (message_t* msg, int* len, int type, fsm_raft_t* raft)
{
  msg->src_id = raft->id[0];
  msg->type = type;
  msg->term = raft->term;
  msg->prev_term = 0; //XXX
  msg->prev_logidx = 0; //XXX
  msg->leader_commit_logidx = 0; //XXX
  *len = sizeof (message_t);
}

void
raft_stats_inc (fsm_raft_t* this, const char* name)
{
  int i;
  for (i = 0; i < this->nstats; ++i) {
    if (strcmp (name, this->stats[i].name) == 0) {
      this->stats[i].value++;
      return;
    }
  }
  this->stats[this->nstats++] = (raft_counter_t) { name, 1 };
}

void raft_stats_reset (fsm_raft_t* this) { this->nstats = 0; }

static message_t* peekMessage(fsm_raft_t* this) {
  return this->msg;
}

static int receivedAppendEntries (fsm_t* this) {
  message_t* msg = peekMessage((fsm_raft_t*) this);
  return msg->type == MSG_APPENDENTRIES;
}

static int receivedReqForVote (fsm_t* this) {
  message_t* msg = peekMessage((fsm_raft_t*) this);
  return msg->type == MSG_REQVOTE;
}

static int receivedVote (fsm_t* this) {
  message_t* msg = peekMessage((fsm_raft_t*) this);
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

<<<<<<< HEAD
static void replyAppendEntries (fsm_t* this) {
  fsm_raft_t* raft = (fsm_raft_t*) this;
  raft_stats_inc (raft, "replyAppendEntries");
=======
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
>>>>>>> 5cb9738a4c0587fba3b982590d1ad6bce7dd3d28
}

static void sendVote (fsm_t* this) {
  fsm_raft_t* raft = (fsm_raft_t*) this;
  raft_stats_inc (raft, "sendVote");
}

static void sendReqForVote (fsm_t* this) {
  fsm_raft_t* raft = (fsm_raft_t*) this;
<<<<<<< HEAD
  int i;
  message_t msg;
  int len;
  message_init (&msg, &len, MSG_REQVOTE, raft);
  for (i = 0; i < raft->nnodes - 1; ++i)
    udp_client_send (raft->neighbor[i], (const char*) &msg, len);
  raft_stats_inc (raft, "sendReqForVote");
}

static void sendAppendEntries (fsm_t* this) {
  fsm_raft_t* raft = (fsm_raft_t*) this;
  raft_stats_inc (raft, "sendAppendEntries");
=======
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
>>>>>>> 5cb9738a4c0587fba3b982590d1ad6bce7dd3d28
}

static void incVotes (fsm_t* this) {
  fsm_raft_t* raft = (fsm_raft_t*) this;
  raft_stats_inc (raft, "incVotes");
  raft->votes++;
}

static void resetFollowerTimeout (fsm_t* this) {
<<<<<<< HEAD
  fsm_raft_t* raft = (fsm_raft_t*) this;
  raft_stats_inc (raft, "resetFollowerTimeout");
=======
  
  printf("resetFollowerTimeout\n");
>>>>>>> 5cb9738a4c0587fba3b982590d1ad6bce7dd3d28
}

static void resetCandidateTimeout (fsm_t* this) {
  fsm_raft_t* raft = (fsm_raft_t*) this;
  raft_stats_inc (raft, "resetCandidateTimeout");
}

static void resetLeaderTimeout (fsm_t* this) {
  fsm_raft_t* raft = (fsm_raft_t*) this;
  raft_stats_inc (raft, "resetLeaderTimeout");
}

<<<<<<< HEAD
fsm_t*
fsm_new_leader (int argc, char* argv[])
=======
static void service_request (fsm_t* this) {
  printf("serviceRequest\n");
}


fsm_t* fsm_new_leader (int id)
>>>>>>> 5cb9738a4c0587fba3b982590d1ad6bce7dd3d28
{
  static fsm_trans_t tt[] =
    {
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
  int i;
  fsm_raft_t* raft = (fsm_raft_t*) malloc (sizeof (fsm_raft_t));
  fsm_t* this = (fsm_t*) raft;
  fsm_init (this, tt);
  raft->nnodes = argc;
  for (i = 0; i < argc; ++i) {
    raft->id[i] = atoi (argv[i]);
  }
  raft->term = 0;
  raft->votes = 0;
  gettimeofday (&raft->next, NULL);

  raft->me = udp_server_new (raft->id[0]);
  for (i = 0; i < raft->nnodes - 1; ++i) {
    raft->neighbor[i] = udp_client_new ("127.0.0.1", raft->id[i+1]);
  }
  raft->nmsg = 0;

  raft_stats_reset (raft);
  return this;
}

static void
raft_get_messages (fsm_raft_t* this)
{
  udp_server_t* srv = this->me;
  struct timeval timeout = { 0, 0 };
  fd_set rdset;
  FD_ZERO (&rdset);
  FD_SET (srv->sockfd, &rdset);
  if (select (srv->sockfd + 1, &rdset, NULL, NULL, &timeout) <= 0)
    return;
  udp_client_t cli;
  int n = udp_server_recv (srv, &cli);
  message_t* msg = this->msg + this->nmsg;
  msg->src_id = ntohs (cli.addr.sin_port);
  msg->type = srv->buffer[0];
  this->nmsg++;
}

static void
raft_dump (fsm_raft_t* this)
{
  int i;
  printf (" nodes: %d-%c ( ", this->id[0], this->fsm.current_state);
  for (i = 1; i < this->nnodes; ++i)
    printf ("%d ", this->id[i]);
  printf (")\n nmsgs: %d:", this->nmsg);
  for (i = 0; i < this->nmsg; ++i)
    printf ("%c ", this->msg[i].type);
  printf ("\n counters:\n");
  for (i = 0; i < this->nstats; ++i) {
    printf ("   %s %d\n", this->stats[i].name, this->stats[i].value);
  }
}

struct interp_raft_t {
  interp_t interp;
  fsm_raft_t* raft;
};
typedef struct interp_raft_t interp_raft_t;

static void do_info (interp_t* this) {
  interp_raft_t* interp = (interp_raft_t*) this;
  raft_dump (interp->raft);
}

interp_t*
interp_raft_new (fsm_raft_t* raft)
{
  interp_raft_t* this = (interp_raft_t*) malloc (sizeof (interp_raft_t));
  interp_init ((interp_t*) this);
  this->raft = raft;
  interp_addcmd ((interp_t*) this, "info", do_info);
  return (interp_t*) this;
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
main (int argc, char* argv[])
{
  fsm_t* fsm_leader = fsm_new_leader (argc - 1, argv + 1);
  //fsm_t* fsm_logrepl = fsm_new_logrepl ((fsm_raft_t *) fsm_leader);
  interp_t* interp = interp_raft_new ((fsm_raft_t*) fsm_leader);
  while (1) {
<<<<<<< HEAD
    raft_get_messages ((fsm_raft_t*) fsm_leader);
    fsm_fire (fsm_leader);
    //fsm_fire (fsm_logrepl);
    interp_run (interp);
=======
    
    fsm_fire (fsm_new_leader);
    fsm_fire (fsm_log_replication);
>>>>>>> 5cb9738a4c0587fba3b982590d1ad6bce7dd3d28
    sleep (1);
  }
}
