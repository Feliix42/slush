#ifndef __COMMAND_H__
#define __COMMAND_H__

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

/// A structure to define a command entered into the shell.
/// The fields `args`, `input_redir`, `output_redir` and `next` may be NULL if
/// not provided by the user.
struct command {
	/// redirection of the input
	char* input_redir;
	/// output redirection
	char* output_redir;
	/// whether the command shall be run in the background
	bool background;
	/// pointer to a linked list of program invocations
	struct program* invocation;
};

struct program {
	/// the command to execute
	char* command;
	/// additional arguments provided to the invocation
	char** args;
	/// if this is a pipe-chain of commands, this links to the next command
	struct program* next;
};


// ------------ functions ------------

int append_invocation(struct command* cmd, char* program);
int add_argument(struct command* cmd, char* argument);

#endif
