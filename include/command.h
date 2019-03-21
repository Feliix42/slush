#ifndef COMMAND_H
#define COMMAND_H

#include <stdbool.h>
#include <stdlib.h>

/// A structure to define a command entered into the shell.
/// The fields `args`, `input_redir`, `output_redir` and `next` may be NULL if
/// not provided by the user.
struct command {
	/// the command to execute
	char* command;
	/// additional arguments provided to the invocation
	char** args;
	/// redirection of the input
	char* input_redir;
	/// output redirection
	char* output_redir;
	/// whether the command shall be run in the background
	bool background;
	/// if this is a pipe-chain of commands, this links to the next command
	struct command* next;
};


#endif
