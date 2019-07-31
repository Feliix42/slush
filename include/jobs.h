#ifndef __JOBS_H__
#define __JOBS_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

#include <environment.h>

void check_bg_jobs(struct environment* env);
void append_job(struct environment* env, pid_t new_pid, pid_t* associated);


#endif /* __JOBS_H__ */
