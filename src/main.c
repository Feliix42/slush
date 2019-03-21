#include <main.h>

#define MAX_CMD_LENGTH 300

struct command** parse_command(const char* expr) {
	yyscan_t scanner;
	YY_BUFFER_STATE state;
	struct command** cmd = calloc(1, sizeof(struct command*));

	if (yylex_init(&scanner)) {
		fprintf(stderr, "Could not initialize scanner!\n");
		return NULL;
	}

	state = yy_scan_string(expr, scanner);

	if (yyparse(cmd, scanner)) {
		// error during parse occured
		fprintf(stderr, "Could not parse input.\n");
		return NULL;
	}

	yy_delete_buffer(state, scanner);
	yylex_destroy(scanner);

	return cmd;
}

// int main(int argc, char **argv) {
int main(void) {
	printf("Welcome to slush - the stupid & lightly underwhelming shell!\n");

	char* input = calloc(MAX_CMD_LENGTH + 1, sizeof(char));

	while (true) {
		char* user = getenv("USER");

		if (user) {
			printf("[%s: slush] ", user);
		} else {
			printf("[slush] ");
		}

		// get input
		if (!fgets(input, MAX_CMD_LENGTH + 1, stdin))
			break;

		struct command** cmd = parse_command(input);

		if (!cmd) {
			fprintf(stderr, "Quitting due to error\n");
			return 1;
		}

		//handle_command(*cmd);

	}

	printf("exit\n");
	return 0;
}
