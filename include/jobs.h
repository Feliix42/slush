#ifndef __JOBS_H__
#define __JOBS_H__

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

#include <environment.h>

/// Prompts the user to confirm the termination of all running jobs.
/// Returns `true` if no jobs are running or all jobs have been terminated and
/// `false` if the user decides against termination.
bool prompt_and_terminate_jobs(struct environment* env);

/// Checks state of all background tasks and reports whether they are done processing.
/// The `force_termination` parameter controls whether the shell waits in a blocking manner for the termination of all processes or not. If set to true, all non-error output from this function is suppressed.
void check_bg_jobs(struct environment* env, bool force_termination);

/// Appends the PID of a background job to the `jobs` array of the environment.
void append_job(struct environment* env, pid_t new_pid, pid_t* associated);


#endif /* __JOBS_H__ */
