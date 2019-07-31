#include <builtins.h>

enum builtin {
	NONE,
	CHDIR,
	PWD,
	KILL,
	ALIAS,
	UNALIAS,
	JOBS,
	RM,
};

/// Prints the help for builtins.
void print_builtins_help() {
	// TODO: Write help
	puts("TODO");
}

enum builtin match_invoc(char* program_name) {
	if (!strcmp(program_name, "cd")) {
		return CHDIR;
	} else if (!strcmp(program_name, "pwd")) {
		return PWD;
	} else if (!strcmp(program_name, "kill")) {
		return KILL;
	} else if (!strcmp(program_name, "alias")) {
		return ALIAS;
	} else if (!strcmp(program_name, "unalias")) {
		return UNALIAS;
	} else if (!strcmp(program_name, "jobs")) {
		return JOBS;
	} else if (!strcmp(program_name, "rm")) {
		return RM;
	} else {
		return NONE;
	}
}

/// Attempts to localize a builtin function with given name and dispatches it, if one is found.
/// Returns `0` if no builtin with given name exists. Otherwise, a process for executing the child is forked and the childs `pid` is returned.
pid_t attempt_to_run_builtin(struct program* invoc, struct environment* env, int in[2], int out[2], struct command* cmd) {
	// match command
	enum builtin builtin_cmd = match_invoc(invoc->command);

	// return 0 if no builtin was invoked
	if (builtin_cmd == NONE) {
		return 0;
	}

	pid_t pid = fork();
	if (pid < 0) {
		fprintf(stderr, "\033[91m[slush: error] Could not fork! %s\033[0m\n", strerror(errno));
		exit(1);
	}

	if (pid == 0) {
		// child
		if (in[0] != -1) {
			// redirect the STDIN_FILENO to pipe
			if (dup2(in[0], STDIN_FILENO) == -1) {
				fprintf(stderr, "\033[91m[slush: error] Unable to open PIPE. %s\033[0m\n", strerror(errno));
				exit(1);
			}
		} else {
			// otherwise this is the first input
			if (cmd->input_redir) {
				if (freopen(cmd->input_redir, "r", stdin) == NULL) {
					fprintf(stderr, "\033[91m[slush: error] Unable to open STDIN file. %s\033[0m\n", strerror(errno));
					exit(1);
				}
			}
		}

		if (out[1] != -1) {
			// redirect the STDOUT_FILENO to pipe
			if (dup2(out[1], STDOUT_FILENO) == -1) {
				fprintf(stderr, "\033[91m[slush: error] Unable to open PIPE. %s\033[0m\n", strerror(errno));
				exit(1);
			}
		} else {
			// otherwise this is the last part of the chain
			if (cmd->output_redir) {
				if (freopen(cmd->output_redir, "w", stdout) == NULL) {
					fprintf(stderr, "\033[91m[slush: error] Unable to open STDOUT file. %s\033[0m\n", strerror(errno));
					exit(1);
				}
			}
		}

		// close unnecessary file descriptors
		close(in[0]);
		close(in[1]);
		close(out[0]);
		close(out[1]);

		// run the builtin
		switch (builtin_cmd) {
		case NONE:
			fprintf(stderr, "\033[91m[slush: error] No builtin - How did that happen?\033[0m\n");
			break;
		case CHDIR:
			fprintf(stderr, "\033[94m[slush: info] `cd` is not yet implemented.\033[0m\n");
			break;
		case PWD:
			pwd(env);
			break;
		case KILL:
			fprintf(stderr, "\033[94m[slush: info] `kill` is not yet implemented.\033[0m\n");
			break;
		case ALIAS:
			fprintf(stderr, "\033[94m[slush: info] `alias` is not yet implemented.\033[0m\n");
			break;
		case UNALIAS:
			fprintf(stderr, "\033[94m[slush: info] `unalias` is not yet implemented.\033[0m\n");
			break;
		case JOBS:
			fprintf(stderr, "\033[94m[slush: info] `jobs` is not yet implemented.\033[0m\n");
			break;
		case RM:
			fprintf(stderr, "\033[94m[slush: info] `rm` is not yet implemented.\033[0m\n");
			break;
		default:
			fprintf(stderr, "\033[91m[slush: error] Unknown builtin - How did that happen?\033[0m\n");
			break;
		}

		exit(EXIT_SUCCESS);
	} else {
		// parent
		return pid;
	}


	return 0;
}
