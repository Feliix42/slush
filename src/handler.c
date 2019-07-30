#include <handler.h>

extern char **environ;

// TODO: Docs
/// returns 0 on error and PID of the new child on success.
pid_t handle_command(struct program* invoc, struct environment* env, int in[2], int out[2], struct command* cmd) {
	char* prog = find_executable(env, invoc->command);
	if (!prog) {
		fprintf(stderr, "\033[93m[slush] Unknown command %s\033[0m\n", invoc->command);
		return 0;
	}

	pid_t pid = fork();

	if (pid < 0) {
		fprintf(stderr, "\033[91m[slush: error] Could not fork! %s\033[0m\n", strerror(errno));
		exit(1);
	}

	if (pid == 0) {
		// child
		if (in[1] != -1) {
			// redirect the STDIN_FILENO to pipe
			if (dup2(in[1], STDIN_FILENO) == -1) {
				fprintf(stderr, "\033[91m[slush: error] Unable to open PIPE. %s\033[0m\n", strerror(errno));
				exit(1);
			}
		} else {
			// otherwise this is the first input
			if (cmd->input_redir) {
				close(STDIN_FILENO);
				if (openat(STDIN_FILENO, cmd->input_redir, O_RDONLY) == -1) {
					fprintf(stderr, "\033[91m[slush: error] Unable to open STDIN file. %s\033[0m\n", strerror(errno));
					exit(1);
				}
			}
		}

		if (out[0] != -1) {
			// redirect the STDOUT_FILENO to pipe
			if (dup2(out[0], STDOUT_FILENO) == -1) {
				fprintf(stderr, "\033[91m[slush: error] Unable to open PIPE. %s\033[0m\n", strerror(errno));
				exit(1);
			}
		} else {
			// otherwise this is the last part of the chain
			if (cmd->output_redir) {
				close(STDOUT_FILENO);
				if (openat(STDOUT_FILENO, cmd->output_redir, O_WRONLY | O_CREAT) == -1) {
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

		// run execve to start the program
		if (execve(prog, invoc->args, environ) == -1) {
			// TODO: maybe(!) err handling
			fprintf(stderr, "\033[91m[slush: error] Failed to execute %s (%s)\033[0m\n", prog, strerror(errno));
			exit(1);
		}
	} else {
		// parent
		free(prog);
		return pid;
	}

	// this should be unreachable
	free(prog);
	return 0;
}

int execute(struct command* cmd, struct environment* env) {
	if (!cmd->invocation) {
		return 1;
	}

	int pipein[2] = {-1, -1};
	int pipeout[2] = {-1, -1};

	size_t len = 0;
	struct program* tmp = cmd->invocation;
	while (tmp != NULL) {
		tmp = tmp->next;
		len++;
	}

	pid_t lead = -1;
	// we allocate one item more than necessary because we want a `-1` terminated list
	pid_t* associated_jobs = calloc(len, sizeof(pid_t));
	int cur_assoc_pos = 0;

	for (struct program* cur = cmd->invocation; cur != NULL; cur = cur->next) {
		if (cur != cmd->invocation) {
			// this is not the first invocation
			pipein[0] = pipeout[0];
			pipein[1] = pipeout[1];
		}

		if (cur->next) {
			if (pipe(pipeout)) {
				fprintf(stderr, "\033[91m[slush: error] Failed to construct a pipe chain (%s)\033[0m\n", strerror(errno));
				// TODO: Error handling
				return 1;
			}
		} else {
			pipeout[0] = -1;
			pipeout[1] = -1;
		}

		pid_t current_pid = handle_command(cur, env, pipein, pipeout, cmd);

		// on error, 0 is returned by `handle_command`
		if (!current_pid) {
			break;
		}

		if (!cur->next) {
			// this is the last PID in row
			associated_jobs[cur_assoc_pos] = -1;
			lead = current_pid;
		} else {
			associated_jobs[cur_assoc_pos] = current_pid;
			cur_assoc_pos++;
		}

		// at last, close unnecessary file descriptors
		close(pipein[1]);
		close(pipeout[0]);
		pipeout[0] = -1;
	}

	if (lead == -1) {
		// Error occured -> Perform cleanup
		for (int i = 0; i < cur_assoc_pos; i++) {
			// deliver SIGKILL to all associated processes and collect them
			kill(associated_jobs[i], SIGKILL);
			waitpid(associated_jobs[i], 0, 0);
		}
		free(associated_jobs);
		return 1;
	}

	// either wait or do background processing
	if (cmd->background) {
		append_job(env, lead, associated_jobs);
	} else {
		waitpid(lead, 0, 0);

		if (cmd->invocation->next) {
			for (int i = 0; associated_jobs[i] != -1; i++) {
				// TODO: Theoretically, all processes should die of a SIGPIPE -> is kill necessary?
				if (waitpid(associated_jobs[i], 0, 0) == -1) {
					fprintf(stderr, "\033[91m[slush: error] An internal error occured! (%s)\033[0m\n", strerror(errno));
					continue;
				}
			}

			free(associated_jobs);
		}
	}

	return 0;
}

/// Checks state of all background tasks and reports whether they are done processing.
void check_bg_jobs(struct environment* env) {
	// TODO: better job handling that also reports other state changes
	// skip when job queue is empty
	if (!env->bg_jobs)
		return;

	// run through the linked list, checking for any job if it's completed
	struct running_job* cur = env->bg_jobs;
	struct running_job* prev = NULL;

	while (cur != NULL) {
		pid_t res = waitpid(cur->job, 0, WNOHANG);

		if (res == 0) {
			prev = cur;
			cur = cur->next;
		 	continue;
		}

		if (res == -1) {
			fprintf(stderr, "\033[91m[slush: error] An internal error occured! (%s)\033[0m\n", strerror(errno));
			prev = cur;
			cur = cur->next;
			continue;
		}

		// when control reaches this point, PID did return
		printf("[slush: info] Job with PID %d has ended.\n", res);
		if (cur->associated) {
			for (int i = 0; cur->associated[i] != -1; i++) {
				// TODO: Theoretically, all processes should die of a SIGPIPE -> is kill necessary?
				if (waitpid(cur->associated[i], 0, 0) == -1) {
					fprintf(stderr, "\033[91m[slush: error] An internal error occured! (%s)\033[0m\n", strerror(errno));
					continue;
				}
			}

			free(cur->associated);
		}

		// update linked list
		struct running_job* tmp = cur;
		if (tmp == env->bg_jobs) {
			// this is the first list element -> prev is still NULL
			env->bg_jobs = cur->next;
			cur = cur->next;
		} else {
			// this is *not* the first list element, normal update is alright
			prev->next = cur->next;
			cur = cur->next;
		}
		free(tmp);
	}
}
