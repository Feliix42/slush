#ifndef __HANDLER_H__
#define __HANDLER_H__

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/errno.h>
#include <sys/wait.h>

#include <builtins.h>
#include <command.h>
#include <environment.h>

int handle_command(struct command* cmd, struct environment* env);
void check_bg_jobs(struct environment* env);

#endif
