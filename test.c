#include "thpool.h"
void* function(void* arg);

threadpool_t tpool;
int main(int argc, char **argv) {
	int i=0;
	int input[] = {1,2,3,4,5,6,7,8,9,10};

	threadpool_init(&tpool, 5, 10);

	for(i=0; i<10; i++) {
		threadpool_addtask(&tpool, function, &input[i]);
	}
	threadpool_destroy(&tpool);
	return 0;
}

void* function(void* arg) {
	printf("Thread [%d] done\n", (*(int *)arg));
	return NULL;
}
