#include <environment.h>

struct environment* initialize_env() {
	struct environment* env = calloc(1, sizeof(struct environment));

	if (!env) {
		return NULL;
	}

	// initialize the PWD path
	char* pwd = calloc(FILENAME_MAX, sizeof(char));
	if (!pwd) {
		free(env);
		return NULL;
	}

	if (!getcwd(pwd, FILENAME_MAX)) {
		free(pwd);
		free(env);
		return NULL;
	}

	env->pwd = pwd;

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
			free(env_path);
			free(pwd);
			free(env);
			return NULL;
		}

		// split the path variable into a string array (Note: last element is NULL!)
		char* token = strtok(env_path, ":");
		int j = 0;
		while (token) {
			path[j] = token;
			j++;
			token = strtok(NULL, ":");
		}

		env->path = path;
	}

	return env;
}

/// Deallocated the environment structure and all associated memory regions.
void deinitialize_env(struct environment* env) {
	if (env->pwd) {
		free(env->pwd);
	}

	if (env->path) {
		free(env->path);
	}

	if (env->jobs) {
		free(env->jobs);
	}

	free(env);
}

/// Iterates through the $PATH variable paths and attempts to find the specified program.
/// Returns either the absolute path to the program or NULL if the program is not in $PATH.
char* find_executable(struct environment* env, char* program) {
	if (!env->path) {
		return NULL;
	}

	char* abspath = malloc(FILENAME_MAX * sizeof(char));
	if (!abspath) {
		fprintf(stderr, "\033[91m[slush: error] Could not allocate memory!\033[0m\n");
		return NULL;
	}

	// iteratively check for every path + program combination and return immediately if it exists
	for (int i = 0; env->path[i] != NULL; i++) {
		strcpy(abspath, env->path[i]);
		if (abspath[strlen(abspath) - 1] != '/') {
			strcat(abspath, "/");
		}

		strcat(abspath, program);

		struct stat buffer;
		if (stat(abspath, &buffer)) {
			continue;
		}

		if ((buffer.st_mode & S_IFMT) == S_IFREG) {
			return abspath;
		}
	}

	free(abspath);
	return NULL;
}

/// Appends the PID of a background job to the `jobs` array of the environment.
void append_job(struct environment* env, pid_t new_pid) {
	if (!env->jobs) {
		env->jobs = calloc(2, sizeof(pid_t));

		if (!env->jobs) {
			fprintf(stderr, "\033[91m[slush: error] Could not allocate memory!\033[0m\n");
			return;
		}

		env->jobs[0] = new_pid;
		env->jobs[1] = -1;
	} else {
		// get size of jobs array
		int len = 1;
		while (env->jobs[len - 1] != -1)
			len++;

		// expand and assign new PID
		env->jobs = realloc(env->jobs, (len + 1) * sizeof(pid_t));
		if (!env->jobs) {
			fprintf(stderr, "\033[91m[slush: error] Could not allocate memory!\033[0m\n");
			return;
		}

		env->jobs[len-1] = new_pid;
		env->jobs[len] = -1;
	}
}
