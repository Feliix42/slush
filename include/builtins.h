#ifndef __BUILTINS_H__
#define __BUILTINS_H__

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

#include <command.h>
#include <environment.h>
#include <builtins/fs.h>
#include <builtins/processes.h>

pid_t attempt_to_run_builtin(struct program* invoc, struct environment* env, int in[2], int out[2], struct command* cmd);

#endif
