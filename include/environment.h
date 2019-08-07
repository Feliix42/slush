#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>

// TODO: Better representation that also includes the original command etc.
/// Structure that represents a single running job, which may be a pipe-chain
struct running_job {
	/// The PID to wait for
	pid_t job;
	/// `-1`-terminated list of associated jobs from the pipe chain
	pid_t* associated;
	/// the command that was used to invoke the command
	char* cmd;
	/// Link to the next element
	struct running_job* next;
};

// TODO: Better sorting of list entries
/// Struct that represents a single item in a linked list of aliases.
/// I am fully aware that it is rather inefficient to use a non-sorted list for this but for now it will do.
struct alias {
	/// The command that is being aliased
	char* command;
	/// The string to replace the input by.
	char* replacement;
	/// Link to the next element
	struct alias* next;
};

struct environment {
	/// Path to the present working directory
	char* pwd;
	/// An array of all paths in the path variable.
	/// Always reserves a NULL element at the end for iteration.
	char** path;
	/// Linked list of PIDs that are processing in the background
	struct running_job* bg_jobs;
	/// A linked list of registered aliases
	struct alias* aliases;
};


struct environment* initialize_env();
void deinitialize_env(struct environment* env);
char* find_executable(struct environment* env, char* program);

#endif
