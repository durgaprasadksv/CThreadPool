#include "thpool.h"
#include <stdlib.h>

/* In this example a number is added to the threadpool 
 * and the task_function just prints a number passed to it
 * Numbers are added to a thread pool and a thread picks up 
 * and executes a task_function for that number. A simple use
 * case to demonstrate the usage of this threadpool. 
 */

void* task_function(void* arg);


int main(int argc, char **argv) {
	int i=0;
	threadpool_t *tpool;
	int input[] = {1,2,3,4,5,6,7,8,9,10};
	
	tpool = (threadpool_t* ) malloc(sizeof(threadpool_t));
	if (NULL == tpool) {
		perror("Unable to malloc threadpoool \n");
		exit(-1);
	}

	if (threadpool_init(tpool, 5, 10) != THREADPOOL_INIT_SUCCESS) {
		perror("Unable to init threadpool");
		free(tpool);
		exit(-1);
	}

	for(i=0; i<10; i++) {
		if (threadpool_addtask(tpool, task_function, &input[i]) != THREADPOOL_ADD_SUCCESS) {
			printf("Unable to add task");
		}
	}
	if (threadpool_destroy(tpool) != THREADPOOL_KILL_SUCCESS) {
		perror("Unable to destroy threadpoool");
	}

	//destroy the memory allocated to threadpool
	free(tpool);
	return 0;
}

void* task_function(void* arg) {
	printf("Thread [%d] done\n", (*(int *)arg));
	return NULL;
}
