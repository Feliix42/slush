#include <main.h>

#define MAX_CMD_LENGTH 400
#ifndef PATH_MAX
#define PROMPT_LENGTH 600
#else
#define PROMPT_LENGTH PATH_MAX + 100
#endif	/* PATH_MAX */

struct command* parse_command(const char* expr) {
	yyscan_t scanner;
	YY_BUFFER_STATE state;
	struct command* cmd = calloc(1, sizeof(struct command));

	if (!cmd) {
		fprintf(stderr, "\033[91m[slush: error] Could not allocate memory!\033[0m\n");
		return NULL;
	}

	if (yylex_init(&scanner)) {
		fprintf(stderr, "\033[91m[slush: error] Could not initialize scanner!\033[0m\n");
		free(cmd);
		return NULL;
	}

	state = yy_scan_string(expr, scanner);

	if (yyparse(cmd, scanner)) {
		// TODO: Error handling https://www.gnu.org/software/bison/manual/html_node/Parser-Function.html
		// error during parse occured
		deinitialize_cmd(cmd);
		return NULL;
	}

	yy_delete_buffer(state, scanner);
	yylex_destroy(scanner);

	return cmd;
}

int main(void) {
	printf("Welcome to slush - the stupid & lightly underwhelming shell!\n");

	char* input = NULL;

	// initialize the environment variable
	struct environment* env = initialize_env();

	if (!env) {
		fprintf(stderr, "\033[91m[slush: error] Could not allocate memory!\033[0m\n");
		return 1;
	}

	// initialize history
	using_history();
	// load old history
	char* hist_file;
	asprintf(&hist_file, "%s/.slush_history", getenv("HOME"));
	if (read_history(hist_file) == ENOENT) {
		int fd = open(hist_file, O_CREAT, S_IRUSR & S_IWUSR);
		if (fd != -1) {
			close(fd);
		} else {
			fprintf(stderr, "\033[94m[slush: warning] Could not store history - %s\033[0m\n", strerror(errno));
		}
	}

	while (true) {
		// check status of any background tasks
		check_bg_jobs(env);

		// build command line string
		char* prompt;
		int result = 0;
		char* user = getenv("USER");
		if (user) {
			// TODO: Use ~ for the pwd
			result = asprintf(&prompt, "[%s: slush] %s ", user, env->pwd);
		} else {
			strcpy(prompt, "[slush] \0");
			result = asprintf(&prompt, "[slush] %s ", env->pwd);
		}

		if (result == -1) {
			fprintf(stderr, "\033[91m[slush: error] Could not allocate memory for prompt!\033[0m\n");
			return 1;
		}

		// get input
		input = readline(prompt);

		if (!input)
			break;

		printf("Input: %s\n", input);

		struct command* cmd = parse_command(input);

		if (!cmd)
			continue;

		if (!cmd->invocation) {
			// exit condition
			deinitialize_cmd(cmd);
			break;
		}

		// add history entry
		add_history(input);

		// TODO: Match return value of command
		handle_command(cmd, env);

		// last step: freeing any allocated memory
		deinitialize_cmd(cmd);
		free(input);
		free(prompt);
	}

	// save and deinitialize history
	int res = write_history(hist_file);
	if (res != 0) {
		fprintf(stderr, "\033[94m[slush: warning] Could not store history - %s\033[0m\n", strerror(res));
	}
	// don't store more than 10.000 history entries
	history_truncate_file(hist_file, 10000);

	deinitialize_env(env);

	printf("exit\n");
	return 0;
}
