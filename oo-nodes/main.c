#include "node.h"

int
main ()
{
  node_t* v1 = node_new_v1 (1);
  node_t* v2 = node_new_v2 (2, 10);
  node_t* c = node_new_composite (3, v1, v2);

  node_send (v1, "hola");
  node_send (v2, "caracola");
  node_send (c, "adios");
}
