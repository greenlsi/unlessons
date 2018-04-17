#include <string.h>
#include <stdio.h>
#include "esp_common.h"
#include "esp_sta.h"
#include "gpio.h"
#include "freertos/task.h"
#include "fsm.h"
#include "raft.h"

#include "lwip/sockets.h"
#include "lwip/tcp.h"

static int port;
static int msock;
static int ssock;
static char buffer[256];
static struct sockaddr_in serv_addr;
static struct sockaddr_in cli_addr;

typedef enum {
    MSG_REQUESTVOTE,
    MSG_REQUESTVOTE_RESPONSE,
    MSG_APPENDENTRIES,
    MSG_APPENDENTRIES_RESPONSE,
} peer_message_type_e;

typedef struct {
    int type;
    union {
        msg_requestvote_t rv;
        msg_requestvote_response_t rvr;
        msg_appendentries_t ae;
        msg_appendentries_response_t aer;
    };
} msg_t;

static msg_t msg;

raft_server_t* raft = NULL;
static int raft_error = 0;
static int error (fsm_t* this) { return raft_error; }
static int no_error (fsm_t* this) { return !raft_error; }

static int always (fsm_t* this) { return 1; }

static int
is_wifi_on (fsm_t* this)
{
    return wifi_station_get_connect_status() == STATION_GOT_IP;
}

static int
is_connection_waiting (fsm_t* this)
{
    fd_set fdset;
    struct timeval timeout = {0, 0};
    FD_ZERO (&fdset);
    FD_SET (msock, &fdset);
    return select (msock + 1, &fdset, NULL, NULL, &timeout) > 0;
}

static int
is_msg_ready (fsm_t* this)
{
    fd_set fdset;
    struct timeval timeout = {0, 0};
    FD_ZERO (&fdset);
    FD_SET (ssock, &fdset);
    return select (ssock + 1, &fdset, NULL, NULL, &timeout) > 0;
}

static int
is_msock_exception (fsm_t* this)
{
    fd_set fdset;
    struct timeval timeout = {0, 0};
    FD_ZERO (&fdset);
    FD_SET (msock, &fdset);
    return select (msock + 1, NULL, NULL, &fdset, &timeout) > 0;
}

static int
is_ssock_exception (fsm_t* this)
{
    fd_set fdset;
    struct timeval timeout = {0, 0};
    FD_ZERO (&fdset);
    FD_SET (ssock, &fdset);
    return select (ssock + 1, NULL, NULL, &fdset, &timeout) > 0;
}

static int msg_appendentries (fsm_t* this) { return msg.type == MSG_APPENDENTRIES; }
static int msg_requestvote (fsm_t* this) { return msg.type == MSG_REQUESTVOTE; }

static void
do_socket (fsm_t* this)
{
    msock = socket (AF_INET, SOCK_STREAM, 0);
    raft_error = msock < 0;
}

static void
do_bind (fsm_t* this)
{
    memset (&serv_addr, 0, sizeof (serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons (port);

    raft_error = bind (msock, (struct sockaddr *) &serv_addr, sizeof (serv_addr));
}

static void
do_listen (fsm_t* this)
{
    listen (msock, 500);
}

static void
do_accept_connection (fsm_t* this)
{
    unsigned clilen = sizeof (cli_addr);
    ssock = accept (msock, (struct sockaddr *) &cli_addr, &clilen);
    raft_error = ssock < 0;
}

static void
do_read_msg (fsm_t* this)
{
    int n = read (ssock, buffer, 255);
    raft_error = n < 0;
}

static void
do_deserialize_msg (fsm_t* this)
{
    /*XXX*/;
}

static void
do_close_connection (fsm_t* this)
{
    close (ssock);
    ssock = -1;
}

static void
handle_appendentries (fsm_t* this)
{
    /*XXX*/;
}

static void
handle_requestvote (fsm_t* this)
{
    /*XXX*/;
}


static int
send_requestvote (raft_server_t* raft, void *user_data,
                  raft_node_t* node, msg_requestvote_t* msg)
{
  printf ("send_requestvote %d -> %d\n", msg->candidate_id, raft_node_get_id(node));
  return 0;
}

static int
send_appendentries (raft_server_t* raft, void *user_data,
                    raft_node_t* node, msg_appendentries_t* msg)
{
  printf ("send_appendentries\n");
  return 0;
}

static int
applylog (raft_server_t* raft, void *udata,
          raft_entry_t *ety, int ety_idx)
{
  printf ("applylog\n");
  return 0;
}

static int
logentry_offer (raft_server_t* raft, void *udata,
                raft_entry_t *ety, int ety_idx)
{
  printf ("logentry_offer\n");
  return 0;
}

static int
logentry_poll (raft_server_t* raft, void *udata,
               raft_entry_t *ety, int ety_idx)
{
  printf ("logentry_poll\n");
  return 0;
}

static int
logentry_pop (raft_server_t* raft, void *udata,
              raft_entry_t *ety, int ety_idx)
{
  printf ("logentry_pop\n");
  return 0;
}

static void
logentry (raft_server_t* raft, raft_node_t* node, void *udata,
          const char *buf)
{
  printf ("LOG: %s\n", buf);
}

static raft_cbs_t raft_funcs = {
  .send_requestvote            = send_requestvote,
  .send_appendentries          = send_appendentries,
  .applylog                    = applylog,
  .log_offer                   = logentry_offer,
  .log_poll                    = logentry_poll,
  .log_pop                     = logentry_pop,
  .log                         = logentry,
};


static void do_raft_periodic (fsm_t* this) { raft_periodic (raft, 250); }

fsm_t*
fsm_new_raft (int p, int id)
{
    static fsm_trans_t tt[] = {
      {  0,  is_wifi_on,            1,  do_socket },
      {  1,  error,                 0,  NULL },
      {  1,  no_error,              2,  do_bind },
      {  2,  error,                 0,  NULL },
      {  2,  no_error,              10, do_listen },
      {  10, is_connection_waiting, 20, do_accept_connection },
      {  10, is_msock_exception,    0,  do_close_connection },
      {  10, always,                10, do_raft_periodic },
      {  20, is_msg_ready,          21, do_read_msg },
      {  21, no_error,              22, do_deserialize_msg },
      {  21, error,                 10, do_close_connection },
      {  22, msg_appendentries,     10, handle_appendentries },
      {  22, msg_requestvote,       10, handle_requestvote },
      {  30, always,                10, do_close_connection },
      {  20, is_ssock_exception,    10, do_close_connection },
      { -1, NULL, -1, NULL },
    };
    port = p;
    msock = ssock = -1;
    raft = raft_new ();
    raft_set_callbacks (raft, &raft_funcs, NULL);
    raft_add_node (raft, NULL, 100, 1);
    raft_add_node (raft, NULL, 101, 0);
    return fsm_new (tt);
}
