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
		// TODO: Error handling https://www.gnu.org/software/bison/manual/html_node/Parser-Function.html
		// error during parse occured
		return NULL;
	}

	yy_delete_buffer(state, scanner);
	yylex_destroy(scanner);

	return cmd;
}

int main(void) {
	printf("Welcome to slush - the stupid & lightly underwhelming shell!\n");

	char* input = NULL;
	size_t linecap = 0;
	// initialize the environment variable
	struct environment* env = initialize_env();

	if (!env) {
		fprintf(stderr, "Failed to allocate memory\n");
		return 1;
	}

	while (true) {
		// check status of any background tasks
		check_bg_jobs(env);

		// print command line
		char* user = getenv("USER");
		if (user) {
			printf("[%s: slush] %s ", user, env->pwd);
		} else {
			printf("[slush] %s ", env->pwd);
		}

		// get input
		linecap = 0;
		if (getline(&input, &linecap, stdin) == -1)
			break;

		// skip empty lines
		if (linecap <= 1 || (linecap == 2 && input[0] == '\n'))
			continue;

		struct command* cmd = parse_command(input);

		if (!cmd)
			continue;

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
