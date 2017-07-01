#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

/***************************************
 *  Tutorial for a single thread create
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

int main()
{
    int ret = 0;
    tinfo_t thread_arg = { 1, "foobar" };

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
