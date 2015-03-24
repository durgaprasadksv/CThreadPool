#include "thpool.h"

TPOOL_STATUS threadpool_init(threadpool_t *tpool, int queue_length, int thread_count) {
	int i;

	if (pthread_mutex_init(&tpool->mutex, NULL) != 0) {
		return THREADPOOL_INIT_ERROR;
	}

	if (pthread_cond_init(&tpool->condvar, NULL) != 0) {
		return THREADPOOL_INIT_ERROR;
	}

	tpool->queue_length = queue_length;
	tpool->thread_count = thread_count;
	tpool->cur_length = -1;
	tpool->kill = 0;

	tpool->queue = (threadpool_task_t *) malloc(sizeof(threadpool_task_t) * queue_length);
	if (NULL == tpool->queue) {
		return THREADPOOL_INIT_ERROR;
	}
	tpool->threads = (pthread_t *) calloc(thread_count, sizeof(pthread_t));
	if (NULL == tpool->threads) {
		return THREADPOOL_INIT_ERROR;
	}

	sem_init(&tpool->sem, 0, queue_length);

	for(i=0; i < thread_count; i++) {
		if (pthread_create(&tpool->threads[i], NULL, task_thread, (void*) tpool) != 0) {
			return THREADPOOL_INIT_ERROR;
		}
	}
	return THREADPOOL_INIT_SUCCESS;
}

TPOOL_STATUS threadpool_join(threadpool_t* tpool) {
	int i;
	for(i=0; i<tpool->queue_length; i++) {
		if (pthread_join(tpool->threads[i], NULL) != 0) {
			return THREADPOOL_JOIN_ERROR;
		}
	}
	return THREADPOOL_JOIN_SUCCESS;
}

TPOOL_STATUS threadpool_addtask(threadpool_t* tpool, void* (*func)(void *), void* args) {

	sem_wait(&tpool->sem);	
	if (pthread_mutex_lock(&tpool->mutex) != 0) {
		return THREADPOOL_LOCK_ERROR;
	}
	
	/* cur_length varies from 0 to queue_length -1 */
	tpool->cur_length = ( tpool->cur_length + 1 ) % (tpool->queue_length);
	tpool->queue[tpool->cur_length].task_func = func;
	tpool->queue[tpool->cur_length].args = args;
	
	if (pthread_mutex_unlock(&tpool->mutex) != 0) {
		return THREADPOOL_LOCK_ERROR;
	}

	if (pthread_cond_signal(&tpool->condvar) != 0) {
		return THREADPOOL_COND_ERROR;
	}

	return THREADPOOL_ADD_SUCCESS;
}

void* task_thread(void *args) {

	threadpool_t* tpool = (threadpool_t *) args;
	threadpool_task_t *task;

	while(1) {

		if (pthread_mutex_lock(&tpool->mutex) != 0) {
			return NULL;
		}
		while((tpool->cur_length < 0) && (!tpool->kill)){
			pthread_cond_wait(&tpool->condvar, &tpool->mutex);
		}
		
		if (tpool->kill) {
			if (tpool->cur_length < 0) {
				if (pthread_mutex_unlock(&tpool->mutex) !=0) {
					return NULL;
				}
				break;
			}
		}

		task = &tpool->queue[tpool->cur_length];
		tpool->cur_length--;

		if (pthread_mutex_unlock(&tpool->mutex) != 0) {
			return NULL;
		}		
		(*(task->task_func))(task->args);
		/* Do a sem post when we are done with that entry in the queue */
		sem_post(&tpool->sem);
	}
	return NULL;
}

TPOOL_STATUS threadpool_destroy(threadpool_t* tpool) {

	int i;

	/* to kill all the threads, set the kill flag and join all threads*/
	if (pthread_mutex_lock(&tpool->mutex) != 0) {
		return THREADPOOL_LOCK_ERROR;
	}
	tpool->kill = 1; /* let threads come out of the loop */
	if (pthread_mutex_unlock(&tpool->mutex) != 0) {
		return THREADPOOL_LOCK_ERROR;
	}

	if (pthread_cond_broadcast(&tpool->condvar) != 0) {
		return THREADPOOL_COND_ERROR;
	}

	for(i=0; i < tpool->thread_count; i++) {
		if (pthread_join(tpool->threads[i], NULL) != 0) {
			return THREADPOOL_KILL_ERROR;
		}
	}

	/* lock the mutex and then destroy it */
	if (pthread_mutex_lock(&tpool->mutex) != 0) {
		return THREADPOOL_LOCK_ERROR;
	}
	pthread_mutex_destroy(&tpool->mutex);
	pthread_cond_destroy(&tpool->condvar);

	free(tpool->queue);
	free(tpool->threads);
	return THREADPOOL_KILL_SUCCESS;
}
