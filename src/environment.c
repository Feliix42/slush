#include <environment.h>

struct environment* initialize_env() {
	struct environment* env = calloc(1, sizeof(struct environment));

	if (!env) {
		return NULL;
	}

	// initialize the CWD path
	char* cwd = calloc(FILENAME_MAX, sizeof(char));
	if (!cwd) {
		return NULL;
	}

	if (!getcwd(cwd, FILENAME_MAX)) {
		return NULL;
	}

	env->cwd = cwd;

	char* env_path = getenv("PATH");

	if (env_path) {
		// count no of paths in the path variable for allocation
		int path_elems = 1;
		for (unsigned long i = 0; i < strlen(env_path); i++) {
			if (env_path[i] == ':')
				path_elems++;
		}

		char** path = calloc(path_elems + 1, sizeof(char*));
		if (!path) {
			return NULL;
		}

		// split the path variable into a string array (Note: last element is NULL!)
		char* token = strtok(env_path, ":");
		int j = 0;
		while(token) {
			path[j] = token;
			j++;
			token = strtok(NULL, ":");
		}
	}

	return env;
}

/// Iterates through the $PATH variable paths and attempts to find the specified program.
/// Returns either the absolute path to the program or NULL if the program is not in $PATH.
char* find_executable(struct environment* env, char* program) {
	if (!env->path) {
		return NULL;
	}

	char* abspath = malloc(FILENAME_MAX * sizeof(char));
	if (!abspath) {
		fprintf(stderr, "Could not allocate memory\n");
		return NULL;
	}

	// iteratively check for every path + program combination and return immediately if it exists
	for (int i = 0; env->path[i] != NULL; i++) {
		strcpy(abspath, env->path[i]);
		strcat(abspath, program);

		struct stat buffer;
		if (!stat(abspath, &buffer)) {
			continue;
		}

		if ((buffer.st_mode & S_IFMT) == S_IFREG) {
			return abspath;
		}
	}

	return NULL;
}
