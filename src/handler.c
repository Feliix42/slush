#include <handler.h>

extern char **environ;

int handle_command(struct command* cmd, struct environment* env) {
	// TODO: expand for more than one program invocation at a time
	char* prog = find_executable(env, cmd->invocation->command);
	if (!prog) {
		fprintf(stderr, "slush: Unknown command %s\n", cmd->invocation->command);
		return 0;
	}

	pid_t pid = fork();

	if (pid < 0) {
		fprintf(stderr, "Error: Could not fork!\n");
		exit(1);
	}

	if (pid == 0) {
		for (int i = 0; cmd->invocation->args[i] != NULL; i++) {
			printf("%s ", cmd->invocation->args[i]);
		}
		printf("\n");

		if (execve(prog, cmd->invocation->args, environ) == -1) {
			// TODO: maybe(!) err handling
			fprintf(stderr, "Failed to execute %s (errno %d)\n", prog, errno);
			exit(1);
		}
	} else {
		waitpid(pid, 0, 0);
	}

	return 1;
}
