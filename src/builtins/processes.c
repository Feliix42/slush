#include <builtins/processes.h>

void kill_process(char* signal, char* pid) {
	(void)signal;
	(void)pid;
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
