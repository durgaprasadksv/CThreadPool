# CThreadPool
CThreadpool is a threadpool implementation in C
Follow the example.c to use it in your own system
The Makefile may be handy to include this implementation
in your own code. 

Node that currently a call to threadpool_destroy waits till
the queue is emptied before the pool is destroyed. The queue
size is bounded and specified at threadpool creation time and 
cannot be changed during runtime. 
 
