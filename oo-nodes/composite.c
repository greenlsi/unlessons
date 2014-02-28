#include <stdio.h>
#include <stdlib.h>
#include "node.h"

struct node_composite_t {
  struct node_t node;
  node_t* n1;
  node_t* n2;
};
typedef struct node_composite_t node_composite_t;

void
node_send_composite (node_t* this, const char* msg)
{
  node_composite_t* n = (node_composite_t*) this;
  
  printf ("%d [composite]: %s\n", this->id, msg);
  printf ("  -> "); node_send (n->n1, msg);
  printf ("  -> "); node_send (n->n2, msg);
}

static struct node_vtbl_t composite_vtbl = {
  node_send_composite
};

void
node_init_composite (node_t* this, int id, node_t* n1, node_t* n2)
{
  node_composite_t* composite = (node_composite_t*) this;
  this->vtbl = &composite_vtbl;
  this->id = id;
  composite->n1 = n1;
  composite->n2 = n2;
}

node_t*
node_new_composite (int id, node_t* n1, node_t* n2)
{
  node_t* this = (node_t*) malloc (sizeof (struct node_composite_t));
  node_init_composite (this, id, n1, n2);
  return this;
}

