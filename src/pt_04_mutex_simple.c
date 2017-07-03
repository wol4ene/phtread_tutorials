#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

/***************************************
 *  Tutorial for a pthread_mutex
 *
 *  In this example, we will have two threads
 *  modifying a shared, global variable.
 *  Each thread will simply add one to the global var
 *  while holding a mutex
 **************************************/

/* shared variable to be used by all threads */
int shared = 5;

/* mutex all threads will use to access shared var */
pthread_mutex_t lock;

/** 
 * Simple struct to use for an argument for the newly
 * created threads.  Obviously any data will work within
 * the struct, this is just an illustrative example.
 */
typedef struct tinfo_s {
    int thread_num;
    char buf[80];
} tinfo_t;

/**
 * Generic thread start routine
 */
void*
thread_func(void *arg) 
{
    tinfo_t *tmp = (tinfo_t*)arg;

    printf("Thread %u - modifying shared data while holding mutex\n",
            tmp->thread_num);

    pthread_mutex_lock(&lock);
    shared++;
    pthread_mutex_unlock(&lock);

    pthread_exit(NULL);
}

#define MAX_THREADS 2
int main()
{
    int ret = 0;
    int i = 0;

    printf("Shared data initially starts at %u. %u threads will inc by one\n",
            shared, MAX_THREADS);

    /* init the mutex */
    ret = pthread_mutex_init(&lock, NULL);
    if (0 != ret) {
        printf("Mutex init failed: %u\n", ret);
        return 0;
    }

    /* Create the args we will pass to each thread */
    tinfo_t thread_args[MAX_THREADS];
    for (i = 0; i < MAX_THREADS; i++) {
        thread_args[i].thread_num = i;
        sprintf(thread_args[i].buf, "Thread Number: %u", i);
    }

    /* Create the thread ids */
    pthread_t tids[MAX_THREADS] = { 0 };

    /* Create each new thread */
    for (i = 0; i < MAX_THREADS; i++) {
        ret = pthread_create(&tids[i], NULL, thread_func, (void*)&thread_args[i]);
        if (0 == ret) {
            printf("Thread %i created succesfully\n", i);
        } else {
            printf("Thread %i create failed: ret=%u\n", i, ret);
        }
    }

    /*
     * Joining allows main to wait for the thread to complete.
     * Without a join, main may exit before the thread gets a chance
     * to run
     */
    for (i = 0; i < MAX_THREADS; i++) {
        pthread_join(tids[i], NULL);
    }

    /* finally, destroy the mutex */
    pthread_mutex_destroy(&lock);

    /* now that all threads have finished, let's see what 
     * the shared data looks like */
    printf("New shared data val: %u\n", shared);

    return 0;
}
