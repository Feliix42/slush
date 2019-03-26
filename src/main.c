#include <main.h>

#define MAX_CMD_LENGTH 400

struct command* parse_command(const char* expr) {
	yyscan_t scanner;
	YY_BUFFER_STATE state;
	struct command* cmd = calloc(1, sizeof(struct command));

	if (!cmd) {
		fprintf(stderr, "Could not allocate memory!\n");
		return NULL;
	}

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

int main(void) {
	printf("Welcome to slush - the stupid & lightly underwhelming shell!\n");

	char* input = calloc(MAX_CMD_LENGTH + 1, sizeof(char));
	// initialize the environment variable
	struct environment* env = initialize_env();

	if (!input || !env) {
		fprintf(stderr, "Failed to allocate memory\n");
		return 1;
	}

	while (true) {
		char* user = getenv("USER");

		if (user) {
			printf("[%s: slush] %s ", user, env->cwd);
		} else {
			printf("[slush] %s ", env->cwd);
		}

		// get input
		if (!fgets(input, MAX_CMD_LENGTH + 1, stdin))
			break;

		struct command* cmd = parse_command(input);

		// TODO: Don't die on parse errors, provide better error handling instead!

		if (!cmd) {
			fprintf(stderr, "Quitting due to error\n");
			return 1;
		}

		if (!cmd->invocation) {
			// exit condition
			break;
		}

		// TODO: Match return value of command
		handle_command(cmd, env);
	}

	printf("exit\n");
	return 0;
}
