#ifndef __BUILTINS_PROCESSES_H__
#define __BUILTINS_PROCESSES_H__

#include <stdio.h>

#include <environment.h>

void kill_process(char* signal, char* pid);
void list_jobs(struct environment* env);

#endif /* __BUILTINS_PROCESSES_H__ */
