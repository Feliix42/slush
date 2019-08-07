#include <jobs.h>

bool prompt_and_terminate_jobs(struct environment* env) {
	// we're done when there are no background jobs
	if (!env->bg_jobs) {
		return true;
	}

	// prompt user
	printf("  There are background jobs running. Exit anyway? [y/N] ");
	char* response = calloc(2, sizeof(char));

	// fgets returns NULL when no input is provided
	if (fgets(response, 2, stdin) && (response[0] == 'y' || response[0] == 'Y')) {
		// kill all jobs & exit
		struct running_job* cur = env->bg_jobs;

		// send a KILL signal to all jobs
		while (cur != NULL) {
			kill(cur->job, SIGKILL);
			cur = cur->next;
		}
		check_bg_jobs(env, true);

		// all bg jobs should be gone now. If not, this is an error.
		if (env->bg_jobs) {
			fprintf(stderr, "\033[91m[slush: error] Not all jobs terminated correctly.\033[0m\n");
		}
		free(response);
		return true;
	} else {
		free(response);
		return false;
	}
}

void check_bg_jobs(struct environment* env, bool force_termination) {
	// TODO: better job handling that also reports other state changes
	// skip when job queue is empty
	if (!env->bg_jobs)
		return;

	// run through the linked list, checking for any job if it's completed
	struct running_job* cur = env->bg_jobs;
	struct running_job* prev = NULL;

	while (cur != NULL) {
		// non-parallel builtins are automatically done
		if (cur->job != -2) {
			pid_t res;
			if (force_termination) {
				res = waitpid(cur->job, 0, 0);
			} else {
				res = waitpid(cur->job, 0, WNOHANG);
			}

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
			if (!force_termination) {
				printf("[slush: info] Job with PID %d has ended.\n", res);
			}
		}

		if (cur->associated) {
			for (int i = 0; cur->associated[i] != -1; i++) {
				// ignore non-parallel builtins
				if (cur->associated[i] != -2) {
					// TODO: Theoretically, all processes should die of a SIGPIPE -> is kill necessary?
					if (waitpid(cur->associated[i], 0, 0) == -1) {
						fprintf(stderr, "\033[91m[slush: error] An internal error occured! (%s)\033[0m\n", strerror(errno));
						continue;
					}
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
		if (tmp->cmd) {
			free(tmp->cmd);
		}
		free(tmp);
	}
}

void append_job(struct environment* env, pid_t new_pid, pid_t* associated, char* invocation) {
	// build the new list item
	struct running_job* jbs = calloc(1, sizeof(struct running_job));
	if (!jbs) {
		fprintf(stderr, "\033[91m[slush: error] Could not allocate memory!\033[0m\n");
		return;
	}

	jbs->job = new_pid;
	if (associated) {
		jbs->associated = associated;
	}
	jbs->cmd = invocation;

	// link into list
	if (!env->bg_jobs) {
		env->bg_jobs = jbs;
	} else {
		// get size of jobs array
		struct running_job* cur = env->bg_jobs;
		while (cur->next != NULL) {
			cur = cur->next;
		}

		cur->next = jbs;
	}
}
