#ifndef USERPROG_PROCESS_H
#define USERPROG_PROCESS_H

#include "threads/thread.h"
#include "threads/synch.h"

#define SAVE 0
#define RETURN 1
volatile int is_wait_called(volatile int command, volatile int tid, volatile int status);
volatile int is_child_exit(volatile int, volatile int, volatile int);

struct semaphore sema_user[2000];
struct semaphore wait_sema[2000];
struct semaphore load_sema;

bool success_load;

tid_t process_execute (const char *file_name);
int process_wait (tid_t);
void process_exit (void);
void process_activate (void);

#endif /* userprog/process.h */
