#include <builtins/alias.h>

#ifndef PATH_MAX
#define PROMPT_LENGTH 600
#else
#define PROMPT_LENGTH PATH_MAX + 100
#endif	/* PATH_MAX */

void add_alias(struct environment* env, int argc, char** argv) {
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

/// Check if a input string corresponds to any known alias. If so, a pointer to the alias struct is returned. If not, NULL is returned.
struct alias* find_alias(struct environment* env, char* cmd) {
	for (struct alias* cur = env->aliases; cur != NULL; cur = cur->next) {
		if (!strcmp(cur->command, cmd)) {
			return cur;
		}
	}

	return NULL;
}

char* apply_aliases(struct environment* env, char* input) {
	// immediately return if there are no aliases
	if (!env->aliases) {
		return input;
	}

    /* BEWARE! There be dragons below here.
     *
     * The following piece of code is a super nasty hack that was written simply
     * because I deemed manual parsing and replacing a string easier than writing
     * a proper substitution that parses aliases (allowing for partial parses and
     * context sensitive parsing -- you'd want to be able to use | in an alias)
     * and merges them back into the parsed `command` data structure.
     *
     * Thus, I use the following:
     * The code below uses the reentrant string tokenizer from `string.h` and
     * searches alternatingly for the next space (to identify the first command
     * in a invocation) and then for the next pipe (to see where an invocation
     * ended when dealing with a pipechain). When the first command for an
     * invocation is found, it's compared to the list of aliases, substituting
     * the alias for the command if one is found.
     *
     * So please, read with care and seek counselling if necessary.
     */

	// savepoint for manual manipulation of reentrant parser
	char** saveptr = &input;
	char* cur_token = strtok_r(input, " ", saveptr);
	char* new_input = calloc(PROMPT_LENGTH, sizeof(char));
	if (!new_input) {
		fprintf(stderr, "\033[91m[slush: error] Could not allocate memory for alias substitution!\033[0m\n");
		return input;
	}

	bool check_for_cmd = true;
	while (cur_token != NULL) {
		if (check_for_cmd) {
			// check the current token for an alias
			while (isspace(cur_token[0]))
				cur_token++;

			struct alias* al = find_alias(env, cur_token);
			if (al) {
				// if a matching alias was found, add the substitution to the new input string
				if ((strlen(new_input) + strlen(al->replacement) + 2) > PROMPT_LENGTH) {
					new_input = realloc(new_input, (strlen(new_input) + strlen(al->replacement) + 2));
					if (!new_input) {
						fprintf(stderr, "\033[91m[slush: error] Could not allocate memory for alias substitution!\033[0m\n");
						return input;
					}
				}

				strcat(new_input, al->replacement);
			} else {
				if ((strlen(new_input) + strlen(cur_token) + 1) > PROMPT_LENGTH) {
					new_input = realloc(new_input, (strlen(new_input) + strlen(cur_token) + 2));
					if (!new_input) {
						fprintf(stderr, "\033[91m[slush: error] Could not allocate memory for alias substitution!\033[0m\n");
						return input;
					}
				}

				if (strlen(new_input) > 0) {
					strcat(new_input, " \0");
				}
				strcat(new_input, cur_token);
			}

			// jump to the next empty space
			check_for_cmd = false;
			if (**saveptr == '|') {
				// hacky workaround to prevent malfunctions when encountering inputs where the cmd before the pipe has no arguments
				(*saveptr)++;
				cur_token = "\0";
			} else {
				cur_token = strtok_r(NULL, "|", saveptr);
			}
		} else {
			// we've just skipped a set of arguments and a Pipe. These need to be appended as is.
			// printf("skipped \"%s\"\n", cur_token);
			if ((strlen(new_input) + strlen(cur_token) + 3) > PROMPT_LENGTH) {
				new_input = realloc(new_input, (strlen(new_input) + strlen(cur_token) + 3));
				if (!new_input) {
					fprintf(stderr, "\033[91m[slush: error] Could not allocate memory for alias substitution!\033[0m\n");
					return input;
				}
			}

			strcat(new_input, " \0");
			strcat(new_input, cur_token);

			// jump to next invocation
			check_for_cmd = true;
			cur_token = strtok_r(NULL, " ", saveptr);
			if (cur_token != NULL) {
				strcat(new_input, "| \0");
			}
		}
	}

	return new_input;
}
