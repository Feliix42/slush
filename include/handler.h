#ifndef HANDLER_H
#define HANDLER_H

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/errno.h>
#include <sys/wait.h>
#include <command.h>
#include <environment.h>

int handle_command(struct command* cmd, struct environment* env);

#endif
