#ifndef __HANDLER_H__
#define __HANDLER_H__

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/types.h>

#include <builtins.h>
#include <command.h>
#include <environment.h>

int execute(struct command* cmd, struct environment* env);

#endif
