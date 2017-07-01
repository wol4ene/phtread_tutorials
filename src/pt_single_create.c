#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

/***************************************
 *  Tutorial for a single thread create
 **************************************/

/**
 * Generic thread start routine
 * Always returns a void* and accepts a void*
 * ALways call pthread_exit() to return
 *
 * For this tutorial, we will assume the input arg is a pointer
 * to an int
 */
void*
thread_func(void *arg) 
{
    printf("Thread %u\n", *(int*)arg);
    pthread_exit(NULL);
}

int main()
{
    int ret = 0;
    int thread_arg = 5;     /* arg to pass to new thread */

    /* Create local pthread variable */
    pthread_t t1 = { 0 };

    /* Create new thread */
    ret = pthread_create(&t1, NULL, thread_func, (void*)&thread_arg);
    if (0 == ret) {
        printf("Thread created succesfully\n");
    } else {
        printf("Thread create failed: ret=%u\n", ret);
    }

    /*
     * Joining allows main to wait for the thread to complete.
     * Without a join, main may exit before the thread gets a chance
     * to run
     */
    pthread_join(t1, NULL);

    return 0;
}
