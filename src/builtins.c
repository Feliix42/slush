#include <builtins.h>

enum builtin {
	NONE,
	CHDIR,
	PWD,
	KILL,
	ALIAS,
	UNALIAS,
	JOBS,
	HELP,
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
	} else if (!strcmp(program_name, "help")) {
		return HELP;
	} else {
		return NONE;
	}
}

/// Attempts to localize a builtin function with given name and dispatches it, if one is found.
/// Returns `0` if no builtin with given name exists. Otherwise, a process for executing the child is forked and the childs `pid` is returned.
/// A number of builtins cannot run in a separate process however, since they need to modify the environment and don't read from/write to stdin/stdout anyway.
pid_t attempt_to_run_builtin(struct program* invoc, struct environment* env, int in[2], int out[2], struct command* cmd) {
	// match command
	enum builtin builtin_cmd = match_invoc(invoc->command);

	// return 0 if no builtin was invoked
	if (builtin_cmd == NONE) {
		return 0;
	}

	// TODO: Redirect output from builtins

	/* pid_t pid = fork();
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
		close(out[1]); */

	size_t argc = 0;
	if (invoc->args) {
		while (invoc->args[argc] != NULL)
			argc++;
	}

	// run the builtin
	switch (builtin_cmd) {
	case NONE:
		fprintf(stderr, "\033[91m[slush: error] No builtin - How did that happen?\033[0m\n");
		break;
	case CHDIR:
		if (argc == 1) {
			change_dir(env, NULL);
		} else {
			change_dir(env, invoc->args[1]);
		}
		break;
	case PWD:
		pwd(env);
		break;
	case KILL:
		kill_process(argc, invoc->args);
		break;
	case ALIAS:
		add_alias(env, argc, invoc->args);
		break;
	case UNALIAS:
		unalias(env, argc, invoc->args);
		break;
	case JOBS:
		list_jobs(env);
		break;
	case HELP:
		print_builtins_help();
		break;
	default:
		fprintf(stderr, "\033[91m[slush: error] Unknown builtin - How did that happen?\033[0m\n");
		break;
	}

		/*
		exit(EXIT_SUCCESS);
	} else {
		// parent
		return pid;
	} */


	return -2;
}
