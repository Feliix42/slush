#ifndef __BUILTINS_H__
#define __BUILTINS_H__

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/errno.h>

#include <environment.h>

void change_dir(struct environment* env, char* target);
void pwd(struct environment* env);
void kill_process(char* signal, char* pid);

#endif
