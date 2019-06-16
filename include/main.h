#ifndef MAIN_H
#define MAIN_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// special-case apple OS'es to make them use libedit
#ifdef __APPLE__
#include <histedit.h>
#else
#include <readline/readline.h>
#include <readline/history.h>
#endif

#include <command.h>
#include <parser.h>
#include <scanner.h>
#include <handler.h>
#include <environment.h>

#endif
