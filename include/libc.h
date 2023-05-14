/*
 * libc.h - macros per fer els traps amb diferents arguments
 *          definició de les crides a sistema
 */
 
#ifndef __LIBC_H__
#define __LIBC_H__

#include <stats.h>
#include <malloc.h>

extern int errno;

int write(int fd, char *buffer, int size);
int read(char* buffer, int size);

int getpid();

int gettime();

int fork();

void exit();

int yield();

int get_stats(int pid, struct stats *st);

int create_thread( void (*function)(void* arg), void* parameter );
void exit_thread();

int mutex_init(int *m);
int mutex_lock(int *m);
int mutex_unlock(int *m);

void* dyn_mem(int num_bytes);
void* sbrk(int num_bytes);

void itoa(int a, char *b);

int strlen(char *a);

void perror();

#endif  /* __LIBC_H__ */
