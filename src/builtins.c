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
void print_builtins_help(FILE* output) {
	// TODO: Write help
	fprintf(output, "TODO\n");
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
	// TODO: Remove someday
	(void)in;

	// match command
	enum builtin builtin_cmd = match_invoc(invoc->command);

	// return 0 if no builtin was invoked
	if (builtin_cmd == NONE) {
		return 0;
	}

	FILE* output = NULL;
	if (out[1] != -1) {
		// output shall be redirected
		output = fdopen(out[1], "w");
	} else {
		if (cmd->output_redir) {
			output = fopen(cmd->output_redir, "w");
		} else {
			// default to write to stdout
			output = stdout;
		}
	}

	if (!output) {
		fprintf(stderr, "\033[91m[slush: error] Unable to open STDOUT. %s\033[0m\n", strerror(errno));
		exit(1);
	}

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
		pwd(env, output);
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
		list_jobs(env, output);
		break;
	case HELP:
		print_builtins_help(output);
		break;
	default:
		fprintf(stderr, "\033[91m[slush: error] Unknown builtin - How did that happen?\033[0m\n");
		break;
	}

	if (output != stdout) {
		fclose(output);
	}

	return -2;
}
