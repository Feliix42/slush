#include <builtins/alias.h>

// TODO: Alias replacement mittels stringsuche auf dem Volltext(?)
void add_alias(struct environment* env, int argc, char** argv) {
	(void)env;
	(void)argc;
	(void)argv;
	// TODO:
	//  - parse input
	//  - make sure alias does not exist yet
	return;
}

void unalias(struct environment* env, int argc, char** argv) {
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
	(void)env;
	(void)input;
	return NULL;
}
