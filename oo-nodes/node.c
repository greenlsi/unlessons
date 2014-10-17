#include "node.h"

/* polymorphic method */
/* this method will invoke the right send implementation for each concrete node,
   by looking for it in the virtual table */
void node_send (node_t* this, const char* msg)
{
  this->vtbl->send (this, msg);
}

