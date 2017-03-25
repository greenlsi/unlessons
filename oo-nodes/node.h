#ifndef NODE_H
#define NODE_H

/* forward declaration */
/* node_t depends on node_vtbl_t and vice versa, this forward declaration is
   necessary to break the mutual dependence */
struct node_t;

/* virtual table for node_t objects */
/* it contains all the virtual methods (polymorphic fuctions) for this type */
/* all node_t objects will have a pointer to the only instance of this table */
struct node_vtbl_t {
  void (*send) (struct node_t*, const char*);
};

/* node "class" */
struct node_t {
  /* the first element is always the virtual table */
  struct node_vtbl_t *vtbl;
  int id;
};
/* this typedef allows omitting struct when creating new nodes */
typedef struct node_t node_t;

/* polymorphic method */
/* this method will invoke the right send implementation for each concrete node,
   by looking for it in the virtual table */
void node_send (node_t* this, const char* msg);

/* constructors */
/* one constructor for each concrete node type */
/* *_new functions will allocate memory and initialize it, while *_init
   *functions will only initialize previously allocated objects */

void node_init_v1 (node_t* this, int id);
node_t* node_new_v1 (int id);

void node_init_v2 (node_t* this, int id, int otro);
node_t* node_new_v2 (int id, int otro);

void node_init_composite (node_t* this, int id, node_t* n1, node_t* n2);
node_t* node_new_composite (int id, node_t* n1, node_t* n2);

#endif
