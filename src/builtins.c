#include <builtins.h>


void change_dir(struct environment* env, char* target) {
	char* new_dir;

	if (!target || strlen(target) == 0) {
		// assume $HOME as fallback
		char* user_home = getenv("HOME");
		if (!user_home) {
			user_home = "/";
		}
		target = user_home;
	}

	if (target[0] == '/') {
		// absolute path
		new_dir = target;
	} else {
		// canonicalize path
		new_dir = strdup(env->pwd);
		if (!new_dir) {
			fprintf(stderr, "Could not allocate memory\n");
			return;
		}

		// manually split and handle path
		char* token = strtok(target, "/");
		while (token) {
			// TODO: impl actual path handling (.., ., foo)
			if (!strcmp(token, ".")) {
				// do nothing
			} else if (!strcmp(token, "..")) {
				// remove last part of path from new_path, always keep leading `/`!
				char* last_slash = strrchr(new_dir, '/');

				// ignore ../ when path is already at "/"
				if (new_dir != last_slash) {
					*last_slash = '\0';
					new_dir = realloc(new_dir, (last_slash - new_dir + 1) / sizeof(char));
				}
			} else {
				char* tmp = new_dir;
				new_dir = malloc(strlen(tmp) + strlen(token) + 2);
				if (!new_dir) {
					fprintf(stderr, "Could not allocate memory\n");
					return;
				}

				strcpy(new_dir, tmp);
				free(tmp);
				strcat(new_dir, "/");
				strcat(new_dir, token);
			}

			// get next token
			token = strtok(NULL, "/");
		}
	}

	// change the directory and update the pwd in the environment
	if (chdir(new_dir) == -1) {
		switch (errno) {
		case EACCES:
			fprintf(stderr, "Permission denied. Please make sure you have the correct access rights.\n");
			break;
		case ELOOP:
			fprintf(stderr, "Target path contains a loop!\n");
			break;
		case ENOENT:
			fprintf(stderr, "The specified directory does not exist.\n");
			break;
		case ENOTDIR:
			fprintf(stderr, "The specified target is not a directory!\n");
			break;
		default:
			fprintf(stderr, "Error (%d)", errno);
			break;
		}
	} else {
		free(env->pwd);
		env->pwd = new_dir;
	}
}

void pwd(struct environment* env) {
	printf("%s\n", env->pwd);
}

void kill_process(char* signal, char* pid) {
	(void)signal;
	(void)pid;
}
