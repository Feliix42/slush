#include <builtins.h>

/// Prints the help for builtins.
void print_builtins_help() {
	// TODO: Write help
	puts("TODO");
}

void change_dir(struct environment* env, char* target) {
	char* new_dir;
	char* user_home = getenv("HOME");
	if (!user_home) {
		user_home = "/";
	}

	if (!target || strlen(target) == 0) {
		// assume $HOME as fallback
		target = user_home;
	}

	// allow ~ as prefix for the user home
	if (target[0] == '~') {
		new_dir = calloc(strlen(env->pwd) + strlen(target), sizeof(char));
		if (!new_dir) {
			fprintf(stderr, "\033[91m[slush: error] Could not allocate memory!\033[0m\n");
			return;
		}

		strcat(new_dir, user_home);
		strcat(new_dir, &target[1]);
	} else {
		new_dir = malloc((strlen(target) + 1) * sizeof(char));
		if (!new_dir) {
			fprintf(stderr, "\033[91m[slush: error] Could not allocate memory!\033[0m\n");
			return;
		}

		strcpy(new_dir, target);
	}

	// change the directory and update the pwd in the environment
	if (chdir(new_dir) == -1) {
		switch (errno) {
		case EACCES:
			fprintf(stderr, "\033[91m[slush: error] Permission denied. Please make sure you have the correct access rights.\033[0m\n");
			break;
		case ELOOP:
			fprintf(stderr, "\033[91m[slush: error] Target path contains a loop!\033[0m\n");
			break;
		case ENOENT:
			fprintf(stderr, "\033[91m[slush: error] The specified directory does not exist.\033[0m\n");
			break;
		case ENOTDIR:
			fprintf(stderr, "\033[91m[slush: error] The specified target is not a directory!\033[0m\n");
			break;
		default:
			fprintf(stderr, "\033[91m[slush: error] %s\033[0m\n", strerror(errno));
			break;
		}
	} else {
		free(env->pwd);
		env->pwd = get_current_dir_name();
	}

	free(new_dir);
}

void pwd(struct environment* env) {
	printf("%s\n", env->pwd);
}

void kill_process(char* signal, char* pid) {
	(void)signal;
	(void)pid;
}
