#include "node.h"

void node_send (node_t* this, const char* msg)
{
  this->vtbl->send (this, msg);
}

