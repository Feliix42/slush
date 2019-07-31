#include <builtins.h>

/// Prints the help for builtins.
void print_builtins_help() {
	// TODO: Write help
	puts("TODO");
}

/// Attempts to localize a builtin function with given name and dispatches it, if one is found.
/// Returns `0` if no builtin with given name exists. Otherwise, a process for executing the child is forked and the childs `pid` is returned.
pid_t attempt_to_run_builtin(struct program* invoc, struct environment* env, int in[2], int out[2], struct command* cmd) {
	return 0;
}
