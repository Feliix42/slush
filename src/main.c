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

	// initialize the environment variable
	struct environment* env = initialize_env();

	if (!env) {
		fprintf(stderr, "Failed to allocate memory\n");
		return 1;
	}

	#ifdef __APPLE__
	// TODO: Handle NULL
	int* linecap = calloc(1, sizeof(int));
	EditLine* el = el_init("slush", stdin, stdout, stderr);
	History* hist = history_init();
	HistEvent* ev = calloc(1, sizeof(HistEvent));
	history(hist, ev, H_SETUNIQUE, 1);

	// TODO: Set up and add history functionality
	// el_set(el, EL_PROMPT, char *(*f)(EditLine *));
	el_set(el, EL_HIST, history, hist);
	#endif

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
		#ifdef __APPLE__
		*linecap = 0;
		input = el_gets(el, linecap);

		if (!input)
			break;

		// add input to the history
		// TODO: Add return value??
		history(hist, ev, H_ENTER, input);
		#else

		input = readline(NULL);

		if (!input)
			break;

		printf("Input: %s\n", input);

		#endif

		/* linecap = 0; */
		/* if (getline(&input, &linecap, stdin) == -1) */
		/* 	break; */

		// skip empty lines
		// if (linecap <= 1 || (linecap == 2 && input[0] == '\n'))
		// 	continue;

		struct command* cmd = parse_command(input);

		if (!cmd)
			continue;

		if (!cmd->invocation) {
			// exit condition
			break;
		}

		// TODO: Match return value of command
		handle_command(cmd, env);

		// last step: freeing the returned string!
		free(input);
	}

	#ifdef __APPLE__
	history_end(hist);
	el_end(el);
	#endif

	printf("exit\n");
	return 0;
}
