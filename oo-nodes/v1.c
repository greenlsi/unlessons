#include <stdio.h>
#include <stdlib.h>
#include "node.h"

/* v1 nodes derive from node with specific send implementation */

/* send implementation for v1 nodes */
/* note this function is not public */
static void
node_send_v1 (node_t* this, const char* msg)
{
  printf ("%d [v1]: %s\n", this->id, msg);
}

/* this is the only instance of the virtual table of v1 nodes */
/* every v1 node will have a pointer to this virtual table */
static struct node_vtbl_t v1_vtbl = {
  .send = node_send_v1
};

/* initialize a v1 node */
/* initialize pointer to virtual table and other data members */
void
node_init_v1 (node_t* this, int id)
{
  this->vtbl = &v1_vtbl;
  this->id = id;
}

/* create a dynamically allocated v1 node */
/* allocate memory and initialize it by calling to the init function */
node_t*
node_new_v1 (int id)
{
  node_t* this = (node_t*) malloc (sizeof (struct node_t));
  node_init_v1 (this, id);
  return this;
}

