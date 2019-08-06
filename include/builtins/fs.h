#ifndef __BUILTINS_FS_H__
#define __BUILTINS_FS_H__

#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <limits.h>

#include <environment.h>

void change_dir(struct environment* env, char* target);
void pwd(struct environment* env, FILE* output);

#endif /* __BUILTINS_FS_H__ */
