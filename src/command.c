//! An invariant that holds for all functions in this file:
//! They return 0 on error and 1 on success.

#include <command.h>

/// Append a new program invocation to the pre-existing list of cmds
/// and set the program name
int append_invocation(struct command* cmd, char* program) {
	struct program* cur = NULL;

	// check if this is the first command
	if (!cmd->invocation) {
		cmd->invocation = calloc(1, sizeof(struct program));
		if (!cmd->invocation) {
			fprintf(stderr, "\033[91m[slush: error] Could not allocate memory!\033[0m\n");
			return 0;
		}
		cur = cmd->invocation;
	} else {
		// otherwise, allocate a new program struct
		// move to last element in queue
		cur = cmd->invocation;
		while (cur->next != NULL) {
			cur = cur->next;
		}

		struct program* new_prog = calloc(1, sizeof(struct program));
		if (!new_prog) {
			fprintf(stderr, "\033[91m[slush: error] Could not allocate memory!\033[0m\n");
			return 0;
		}
		cur->next = new_prog;
		cur = new_prog;
	}

	// set the program name
	cur->command = program;
	return 1;
}

/// Append an argument to the current (i.e., last) program invocation
int add_argument(struct command* cmd, char* argument) {
	// localize the current program invocation
	struct program* cur = cmd->invocation;
	while (cur->next != NULL) {
		cur = cur->next;
	}

	if (!cur->args) {
		// this is the first argument
		cur->args = calloc(2, sizeof(char*));
		if (!cur->args) {
			fprintf(stderr, "\033[91m[slush: error] Could not allocate memory!\033[0m\n");
			return 0;
		}
		cur->args[0] = argument;
	} else {
		// this is the next argument
		int len = 0;
		while (cur->args[len] != NULL) {
			len++;
		}

		cur->args = realloc(cur->args, (len + 2) * sizeof(char*));
		if (!cur->args) {
			fprintf(stderr, "\033[91m[slush: error] Could not allocate memory!\033[0m\n");
			return 0;
		}

		cur->args[len] = argument;
		cur->args[len+1] = NULL;
	}

	return 1;
}

/// Iteratively frees all memory allocated for the `cmd` data structure.
void deinitialize_cmd(struct command* cmd) {
	if (cmd->input_redir) {
		free(cmd->input_redir);
	}

	if (cmd->output_redir) {
		free(cmd->output_redir);
	}

	struct program* cur = cmd->invocation;
	while (cur != NULL) {
		struct program* this = cur;

		// this->command itself must not be freed as it is also part of the `args` data structure
		if (this->args) {
			int len = 0;
			while (this->args[len] != NULL) {
				free(this->args[len]);
				len++;
			}

			free(this->args);
		}

		cur = this->next;
		free(this);
	}

	free(cmd);
}
