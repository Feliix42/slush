#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>

struct environment {
	/// Path to the present working directory
	char* pwd;
	/// An array of all paths in the path variable.
	/// Always reserves a NULL element at the end for iteration.
	char** path;
	// TODO: Better representation that also includes the original command etc.
	/// `-1`-terminated array of PIDs that are processing in the background
	pid_t* jobs;
};

struct environment* initialize_env();
void deinitialize_env(struct environment* env);
char* find_executable(struct environment* env, char* program);
void append_job(struct environment* env, pid_t new_pid);

#endif
