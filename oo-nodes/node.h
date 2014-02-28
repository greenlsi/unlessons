#ifndef NODE_H
#define NODE_H

struct node_t;

struct node_vtbl_t {
  void (*send) (struct node_t*, const char*);
};

struct node_t {
  struct node_vtbl_t *vtbl;
  int id;
};
typedef struct node_t node_t;

void node_send (node_t* this, const char* msg);


void node_init_v1 (node_t* this, int id);
node_t* node_new_v1 (int id);

void node_init_v2 (node_t* this, int id, int otro);
node_t* node_new_v2 (int id, int otro);

void node_init_composite (node_t* this, int id, node_t* n1, node_t* n2);
node_t* node_new_composite (int id, node_t* n1, node_t* n2);

#endif
