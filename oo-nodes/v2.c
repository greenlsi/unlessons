#include <stdio.h>
#include <stdlib.h>
#include "node.h"

struct node_v2_t {
  struct node_t node;
  int otro;
};
typedef struct node_v2_t node_v2_t;

void
node_send_v2 (node_t* this, const char* msg)
{
  printf ("%d [v2]: %s\n", this->id, msg);
}

static struct node_vtbl_t v2_vtbl = {
  node_send_v2
};

void
node_init_v2 (node_t* this, int id, int otro)
{
  node_v2_t* v2 = (node_v2_t*) this;
  this->vtbl = &v2_vtbl;
  this->id = id;
  v2->otro = otro;
}

node_t*
node_new_v2 (int id, int otro)
{
  node_t* this = (node_t*) malloc (sizeof (struct node_v2_t));
  node_init_v2 (this, id, otro);
  return this;
}

