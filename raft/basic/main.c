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

static void replyAppendEntries (fsm_t* this) {
  fsm_raft_t* raft = (fsm_raft_t*) this;
  raft_stats_inc (raft, "replyAppendEntries");
}

static void sendVote (fsm_t* this) {
  fsm_raft_t* raft = (fsm_raft_t*) this;
  raft_stats_inc (raft, "sendVote");
}

static void sendReqForVote (fsm_t* this) {
  fsm_raft_t* raft = (fsm_raft_t*) this;
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
}

static void incVotes (fsm_t* this) {
  fsm_raft_t* raft = (fsm_raft_t*) this;
  raft_stats_inc (raft, "incVotes");
  raft->votes++;
}

static void resetFollowerTimeout (fsm_t* this) {
  fsm_raft_t* raft = (fsm_raft_t*) this;
  raft_stats_inc (raft, "resetFollowerTimeout");
}

static void resetCandidateTimeout (fsm_t* this) {
  fsm_raft_t* raft = (fsm_raft_t*) this;
  raft_stats_inc (raft, "resetCandidateTimeout");
}

static void resetLeaderTimeout (fsm_t* this) {
  fsm_raft_t* raft = (fsm_raft_t*) this;
  raft_stats_inc (raft, "resetLeaderTimeout");
}

fsm_t*
fsm_new_leader (int argc, char* argv[])
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

int
main (int argc, char* argv[])
{
  fsm_t* fsm_leader = fsm_new_leader (argc - 1, argv + 1);
  //fsm_t* fsm_logrepl = fsm_new_logrepl ((fsm_raft_t *) fsm_leader);
  interp_t* interp = interp_raft_new ((fsm_raft_t*) fsm_leader);
  while (1) {
    raft_get_messages ((fsm_raft_t*) fsm_leader);
    fsm_fire (fsm_leader);
    //fsm_fire (fsm_logrepl);
    interp_run (interp);
    sleep (1);
  }
}
