#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>

struct environment {
	/// Path to the current working directory
	char* cwd;
	/// An array of all paths in the path variable. Always reserves a NULL element at the end for iteration.
	char** path;
};

struct environment* initialize_env();
char* find_executable(struct environment* env, char* program);

#endif
