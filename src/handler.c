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
	pid_t* associated_jobs = NULL;
	if (len != 0) {
		associated_jobs = calloc(len, sizeof(pid_t));
	}
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

		// attempt to run the cmd as builtin first
		pid_t current_pid = attempt_to_run_builtin(cur, env, pipein, pipeout, cmd);

		// if no matching builtin can be spawned, it must be a normal cmd
		if (!current_pid) {
			current_pid = handle_command(cur, env, pipein, pipeout, cmd);
		}

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
		close(pipein[0]);
		close(pipeout[1]);
		pipeout[1] = -1;
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
		if (lead != -2) {
			waitpid(lead, 0, 0);
		}

		if (cmd->invocation->next) {
			for (int i = 0; associated_jobs[i] != -1; i++) {
				// TODO: Theoretically, all processes should die of a SIGPIPE -> is kill necessary?
				if (associated_jobs[i] != -2) {
					if (waitpid(associated_jobs[i], 0, 0) == -1) {
						fprintf(stderr, "\033[91m[slush: error] An internal error occured! (%s)\033[0m\n", strerror(errno));
						continue;
					}
				}
			}
		}
		if (associated_jobs) {
			free(associated_jobs);
		}
	}

	return 0;
}
