#include <builtins/alias.h>

void add_alias(struct environment* env, int argc, char** argv) {
	for (int i = 0; i < argc; i++) {
		printf("Argument %d: %s\n", i, argv[i]);
	}

	if (argc != 3) {
		fprintf(stderr, "alias: Incorrect number of aliases!\n"
			"Usage: alias <name> <command>\n");
		return;
	}

	struct alias* new = calloc(1, sizeof(struct alias));
	if (!new) {
		fprintf(stderr, "\033[91m[slush: error] Could not allocate memory for new alias!\033[0m\n");
		return;
	}

	new->command = strdup(argv[1]);
	new->replacement = strdup(argv[2]);

	if (!env->aliases) {
		env->aliases = new;
	} else {
		struct alias* cur = env->aliases;
		if (!strcmp(cur->command, new->command)) {
			// a duplicate already exists
			fprintf(stderr, "alias: %s is already aliased to \"%s\"\n", cur->command, cur->replacement);
			free(new->command);
			free(new->replacement);
			free(new);
			return;
		}

		while (cur->next != NULL) {
			if (!strcmp(cur->command, new->command)) {
				// a duplicate already exists
				fprintf(stderr, "alias: %s is already aliased to \"%s\"\n", cur->command, cur->replacement);
				free(new->command);
				free(new->replacement);
				free(new);
				return;
			}
			cur = cur->next;
		}

		cur->next = new;
	}

	return;
}

void unalias(struct environment* env, int argc, char** argv) {
	if (!env->aliases) {
		fprintf(stderr, "unalias: There are no aliases defined\n");
		return;
	}

	if (argc < 2) {
		fprintf(stderr, "unalias: Insufficient number of arguments!\n"
		"Usage: unalias [alias [...]]\n");
	}

	for (int i = 1; i < argc; i++) {
		bool removed = false;
		struct alias* cur = env->aliases;
		struct alias* prev = NULL;

		while (cur != NULL) {
			// check for matching alias name
			if (!strcmp(cur->command, argv[i])) {
				// found the entry!

				// update linked list to remove the current entry
				struct alias* tmp = cur;
				if (tmp == env->aliases) {
					// this is the first list element -> prev is still NULL
					env->aliases = cur->next;
					cur = cur->next;
				} else {
					// this is *not* the first list element, normal update is alright
					prev->next = cur->next;
					cur = cur->next;
				}
				free(tmp);
				removed = true;
				break;
			} else {
				prev = cur;
				cur = cur->next;
			}
		}

		if (!removed) {
			fprintf(stderr, "unalias: No alias of name %s found.\n", argv[i]);
		}
	}
	return;
}

char* apply_aliases(struct environment* env, char* input) {
	// TODO: Alias replacement mittels stringsuche auf dem Volltext(?)
	(void)env;
	(void)input;
	return NULL;
}
