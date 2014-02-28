#include <stdio.h>
#include <stdlib.h>
#include "node.h"

void
node_send_v1 (node_t* this, const char* msg)
{
  printf ("%d [v1]: %s\n", this->id, msg);
}

static struct node_vtbl_t v1_vtbl = {
  node_send_v1
};

void
node_init_v1 (node_t* this, int id)
{
  this->vtbl = &v1_vtbl;
  this->id = id;
}

node_t*
node_new_v1 (int id)
{
  node_t* this = (node_t*) malloc (sizeof (struct node_t));
  node_init_v1 (this, id);
  return this;
}

