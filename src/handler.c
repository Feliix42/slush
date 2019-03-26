#include <handler.h>

int handle_command(struct command* cmd, struct environment* env) {
	// TODO: expand for more than one program invocation at a time
	char* prog = find_executable(env, cmd->invocation->command);
	if (!prog) {
		fprintf(stderr, "slush: Unknown command %s\n", cmd->invocation->command);
		return 0;
	}

	puts("I know this program!\n");
	return 1;
}
