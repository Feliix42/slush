#include <jobs.h>

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
		// non-parallel builtins are automatically done
		if (cur->job != -2) {
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
		free(tmp);
	}
}

/// Appends the PID of a background job to the `jobs` array of the environment.
void append_job(struct environment* env, pid_t new_pid, pid_t* associated) {
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
