#include <stdio.h>
#include <stdlib.h>
#include "node.h"

/* v2 nodes derive from node adding extra data members */

/* v2 node "class", derived from node "class" */
struct node_v2_t {
  /* the first element is a node to ensure a pointer to a v2 is also a pointer
     to a node */
  struct node_t node;
  int otro;
};
typedef struct node_v2_t node_v2_t;

/* send implementation for v2 nodes */
/* note this function is not public */
static void
node_send_v2 (node_t* this, const char* msg)
{
  node_v2_t* v2 = (node_v2_t*) this;
  printf ("%d [v2] (otro=%d): %s\n", v2->otro, this->id, msg);
}

/* this is the only instance of the virtual table of v2 nodes */
/* every v2 node will have a pointer to this virtual table */
static struct node_vtbl_t v2_vtbl = {
  .send = node_send_v2
};

/* initialize a v2 node */
/* initialize pointer to virtual table and other data members */
void
node_init_v2 (node_t* this, int id, int otro)
{
  node_v2_t* v2 = (node_v2_t*) this;
  node_init_v1(this, id);
  this->vtbl = &v2_vtbl;
  v2->otro = otro;
}

/* create a dynamically allocated v2 node */
/* allocate memory and initialize it by calling to the init function */
node_t*
node_new_v2 (int id, int otro)
{
  node_t* this = (node_t*) malloc (sizeof (struct node_v2_t));
  node_init_v2 (this, id, otro);
  return this;
}

