#ifndef __BUILTINS_ALIAS_H__
#define __BUILTINS_ALIAS_H__

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include <environment.h>

/// Add a new alias to the list of existing aliases
void add_alias(struct environment* env, int argc, char** argv);

/// Remove an alias from the list of known aliases.
/// `argv` may also be a list of aliases.
void unalias(struct environment* env, int argc, char** argv);

/// Apply any aliases to the input string read from stdin. If any aliases are found, they're substituted in the input string and a newly allocated string is returned. If no aliases are found, `input` is returned.
char* apply_aliases(struct environment* env, char* input);

#endif /* __BUILTINS_ALIAS_H__ */
