
#ifndef __THPOOL_H__
#define __THPOOL_H__

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <setjmp.h>
#include <signal.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <errno.h>
#include <math.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>


typedef enum TPOOL_STATUS
{
	THREADPOOL_INIT_ERROR,
	THREADPOOL_INIT_SUCCESS,
	THREADPOOL_ADD_ERROR,
	THREADPOOL_ADD_SUCCESS,
	THREADPOOL_LOCK_ERROR,
	THREADPOOL_LOCK_SUCCESS,
	THREADPOOL_COND_ERROR,
	THREADPOOL_COND_SUCCESS,
	THREADPOOL_KILL_ERROR,
	THREADPOOL_KILL_SUCCESS,
	THREADPOOL_JOIN_ERROR,
	THREADPOOL_JOIN_SUCCESS,
	
}TPOOL_STATUS;

typedef struct threadpool_task_t_ {
	void* args;
	void* (*task_func)(void*);
} threadpool_task_t;

typedef struct threadpool_t_ {
	pthread_mutex_t mutex;
	pthread_cond_t condvar;
	threadpool_task_t *queue;
	int queue_length;
	int cur_length;
	int thread_count;
	int kill;
	pthread_t *threads;
	sem_t sem;
} threadpool_t;

void* task_thread(void *args);
TPOOL_STATUS threadpool_init(threadpool_t *tpool,int queue_length, int thread_count);
TPOOL_STATUS threadpool_destroy(threadpool_t* tpool);
TPOOL_STATUS threadpool_addtask(threadpool_t* tpool, void* (*func)(void *), void* args);

#endif
