#include <stdio.h>
#include <stdlib.h>
#include "node.h"

/* composite nodes derive also from node and contain references to other 2
   nodes */
/* send invocations on composite nodes cause send to be invoked on the 2
   referenced nodes */

struct node_composite_t {
  /* the first element is a node to ensure a pointer to a composite is also a
     pointer to a node */
  struct node_t node;
  node_t* n1;
  node_t* n2;
};
typedef struct node_composite_t node_composite_t;

/* send implementation for composite nodes: call send on the inner nodes */
/* note this function is not public */
static void
node_send_composite (node_t* this, const char* msg)
{
  node_composite_t* n = (node_composite_t*) this;
  
  printf ("%d [composite]: %s\n", this->id, msg);
  printf ("  -> "); node_send (n->n1, msg);
  printf ("  -> "); node_send (n->n2, msg);
}

/* this is the only instance of the virtual table of composite nodes */
/* every composite node will have a pointer to this virtual table */
static struct node_vtbl_t composite_vtbl = {
  node_send_composite
};

/* initialize a composite node */
/* initialize pointer to virtual table and other data members */
void
node_init_composite (node_t* this, int id, node_t* n1, node_t* n2)
{
  node_composite_t* composite = (node_composite_t*) this;
  this->vtbl = &composite_vtbl;
  this->id = id;
  composite->n1 = n1;
  composite->n2 = n2;
}

/* create a dynamically allocated composite node */
/* allocate memory and initialize it by calling to the init function */
node_t*
node_new_composite (int id, node_t* n1, node_t* n2)
{
  node_t* this = (node_t*) malloc (sizeof (struct node_composite_t));
  node_init_composite (this, id, n1, n2);
  return this;
}

