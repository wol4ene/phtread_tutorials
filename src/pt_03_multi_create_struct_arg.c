#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

/***************************************
 *  Tutorial for a multi thread create
 *  that takes a general struct arg
 **************************************/

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

    printf("tinfo.thread_num: %u\n", tmp->thread_num);
    printf("tinfo.buf: '%s'\n", tmp->buf);
    pthread_exit(NULL);
}

#define MAX_THREADS 10
int main()
{
    int ret = 0;
    int i = 0;

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

    return 0;
}
