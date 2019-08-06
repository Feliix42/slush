#ifndef __BUILTINS_PROCESSES_H__
#define __BUILTINS_PROCESSES_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <signal.h>
#include <errno.h>

#include <environment.h>

/// Slush builtin: Send a signal to a running process.
///
/// Calling convention (from the shell): `kill [-<signum>] pid`
/// If signum is given, the correspondig signal (system dependant!) is sent to the process.
/// Otherwise, SIGTERM is sent.
void kill_process(int argc, char** args);

/// Slush builtin: Print a list of all running jobs in the current shell.
void list_jobs(struct environment* env, FILE* output);

#endif /* __BUILTINS_PROCESSES_H__ */
