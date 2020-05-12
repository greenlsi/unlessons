#ifndef INTERP_H
#define INTERP_H

void interp_run (void);
int interp_addcmd (char* cmd, int (*cmd_func) (char*), char* doc);

#endif
