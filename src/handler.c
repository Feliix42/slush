#include <handler.h>

extern char **environ;

int handle_command(struct command* cmd, struct environment* env) {
	// TODO: expand for more than one program invocation at a time
	char* prog = find_executable(env, cmd->invocation->command);
	if (!prog) {
		fprintf(stderr, "\033[93m[slush] Unknown command %s\033[0m\n", cmd->invocation->command);
		return 0;
	}

	pid_t pid = fork();

	if (pid < 0) {
		fprintf(stderr, "\033[91m[slush: error] Could not fork! %s\033[0m\n", strerror(errno));
		exit(1);
	}

	if (pid == 0) {
		// child
		if (cmd->input_redir) {
			close(STDIN_FILENO);
			if (openat(STDIN_FILENO, cmd->input_redir, O_RDONLY) == -1) {
				fprintf(stderr, "\033[91m[slush: error] Unable to open STDIN file. %s\033[0m\n", strerror(errno));
				exit(1);
			}
		}

		if (cmd->output_redir) {
			close(STDOUT_FILENO);
			if (openat(STDOUT_FILENO, cmd->output_redir, O_WRONLY | O_CREAT) == -1) {
				fprintf(stderr, "\033[91m[slush: error] Unable to open STDOUT file. %s\033[0m\n", strerror(errno));
				exit(1);
			}
		}

		// run execve to start the program
		if (execve(prog, cmd->invocation->args, environ) == -1) {
			// TODO: maybe(!) err handling
			fprintf(stderr, "\033[91m[slush: error] Failed to execute %s (%s)\033[0m\n", prog, strerror(errno));
			exit(1);
		}
	} else {
		if (!cmd->background) {
			// TODO: One could catch the exit status and return it to the user
			waitpid(pid, 0, 0);
		} else {
			append_job(env, pid);
		}
	}

	free(prog);
	return 1;
}

int execute(struct command* cmd, struct environment* env) {
	for (struct program* cur = cmd->invocation; cur != NULL; cur = cur->next) {
		// in chain: fork, pipe outputs to next element (if any) and make every process in the chain wait for the next one
		// TODO
	}


	// TODO: Remove:
	(void)env;
	return 0;
}

/// Checks state of all background tasks and reports whether they are done processing.
void check_bg_jobs(struct environment* env) {
	// TODO: better job handling that also reports other state changes
	// skip when job queue is empty
	if (!env->jobs)
		return;

	// track the size of the array in i
	int i;
	for (i = 0; env->jobs[i] != -1; i++) {
		pid_t res = waitpid(env->jobs[i], 0, WNOHANG);

		if (res == 0)
			continue;

		if (res == -1) {
			fprintf(stderr, "\033[91m[slush: error] An internal error occured! (errno %d)\033[0m\n", errno);
			continue;
		}

		// when control reaches this point, PID did return
		printf("Job with PID %d has ended.\n", res);
		env->jobs[i] = 0;
	}

	// cleanup
	// new_len: counts number of remaining jobs
	int new_len = 0;
	for (int j = 0; env->jobs[j] != -1; j++) {
		if (env->jobs[j] != 0) {
			// move any non-null PIDs to the front of the array
			if (j != new_len) {
				env->jobs[new_len] = env->jobs[j];
				env->jobs[j] = 0;
			}
			new_len++;
		}
	}

	// reallocate memory
	if (new_len == 0) {
		free(env->jobs);
		env->jobs = NULL;
	} else {
		if ((new_len + 1) != i) {
			// call must succeed since we shrink the memory
			env->jobs = realloc(env->jobs, (new_len + 2) * sizeof(pid_t));
			env->jobs[new_len + 1] = -1;
		}
	}
}
