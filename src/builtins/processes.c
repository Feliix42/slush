#include <builtins/processes.h>

void kill_process(int argc, char** args) {
	// we require at least one argument
	if (argc < 2) {
		fprintf(stderr, "kill: Insufficient number of arguments!\n"
			"Usage: kill [-<signum>] pid\n");
		return;
	}

	int signum = SIGTERM;

	if (argc == 3) {
		// a signal number was given
		if (args[1][0] != '-') {
			fprintf(stderr, "kill: Incorrect format for signum!\n"
				"Usage: kill [-<signum>] pid\n");
			return;
		}
		signum = atoi(&args[1][1]);
		if (signum == 0 && args[1][1] != '0') {
			fprintf(stderr, "kill: Invalid signal number provided!\n");
			return;
		}
	} else if (argc > 3) {
		// more than 3 arguments make no sense
		fprintf(stderr, "kill: Too many arguments!\n"
			"Usage: kill [-<signum>] pid\n");
		fprintf(stderr, "[DEBUG] Argument count: %d\n", argc);
		for (int i = 0; i < argc; i++) {
			fprintf(stderr, "arg %d: %s", i, args[i]);
		}
		return;
	}

	pid_t target = atoi(args[argc-1]);
	if (target == 0 && args[argc-1][0] != '0') {
		fprintf(stderr, "kill: Illegal value provided for process id!\n"
			"Usage: kill [-<signum>] pid\n");
		return;
	}

	if (kill(target, signum)) {
		// an error occured
		fprintf(stderr, "kill: Unable to deliver signal: %s\n", strerror(errno));
	}

	// TODO:
	//  - later add signal names etc
}

void list_jobs(struct environment* env) {
	if (!env || !env->bg_jobs) {
		puts("There are no running jobs.");
		return;
	}

	// iterate over all background jobs, print PID list
	puts("Active jobs:\n");
	for (struct running_job* cur = env->bg_jobs; cur != NULL; cur = cur->next) {
		if (cur->job != -2) {
			printf("  %d\n", cur->job);
		}
	}
}
