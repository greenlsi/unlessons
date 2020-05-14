#ifndef REACTOR_H
#define REACTOR_H

#include <sys/time.h>

int timeval_less (const struct timeval* a, const struct timeval* b);
void timeval_add (struct timeval* res,
                  const struct timeval* a, const struct timeval* b);
void timeval_sub (struct timeval* res,
                  const struct timeval* a, const struct timeval* b);


struct event_handler_t;
typedef void (*eh_func_t) (struct event_handler_t*);

struct event_handler_t {
  int prio;
  int fd;
  struct timeval next_activation;
  eh_func_t handle_timeout;
  eh_func_t handle_read;
  eh_func_t handle_write;
  eh_func_t handle_exception;
};
typedef struct event_handler_t event_handler_t;

void event_handler_init (event_handler_t* eh, int prio, int fd,
			 eh_func_t handle_timeout,
			 eh_func_t handle_read,
			 eh_func_t handle_write,
			 eh_func_t handle_exception);
void event_handler_handle_timeout (event_handler_t* eh);
void event_handler_handle_read (event_handler_t* eh);
void event_handler_handle_write (event_handler_t* eh);
void event_handler_handle_exception (event_handler_t* eh);

void reactor_init (void);
void reactor_add_handler (event_handler_t* eh);
void reactor_handle_events (void);

typedef event_handler_t* (*eh_new_fd_t) (int);
event_handler_t* eh_new_acceptor (int prio, int port, eh_new_fd_t eh_new);

#endif
