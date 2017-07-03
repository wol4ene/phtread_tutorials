#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

/***************************************
 *  Tutorial for a pthread_mutex deadlock
 *
 *  In this example, we will have two threads and two 
 *  locks.  Through the use of sleeps, we will force
 *  the classic deadlock situation:
 *   Thread 1 takes Lock1, yields to Thread 2
 *   Thread 2 takes Lock2
 *   Thread 2 tries to take Lock1, blocks, yields to Thread 1
 *   Thread 1 tries to take Lock2, blocks
 *    ---->  DEADLOCK
 **************************************/

/* two mutexes used to illustrate deadlock */
pthread_mutex_t Lock1;
pthread_mutex_t Lock2;

/**
 * Thread1 - will take Lock1, sleep to yield to Thread2,
 *           then try to take Lock2.
 */
void*
thread1_func(void *arg) 
{
    printf("Thread 1 taking Lock1, sleeping 5 to yield to Thread2\n");
    pthread_mutex_lock(&Lock1);
    sleep(5);
    printf("Thread 1 awoken, trying to take Lock2, will deadlock...\n");
    pthread_mutex_lock(&Lock2);

    pthread_exit(NULL);
}

/**
 * Thread2 - Will take Lock1, then try to take Lock1, which will block
 */
void*
thread2_func(void *arg) 
{
    printf("Thread 2 taking Lock2 then Lock1, which will block\n");

    pthread_mutex_lock(&Lock2);
    pthread_mutex_lock(&Lock1);

    pthread_exit(NULL);
}

#define MAX_THREADS 2
int main()
{
    int ret = 0;
    int i = 0;

    /* init the mutexex */
    if (0 != pthread_mutex_init(&Lock1, NULL)) {
        printf("Mutex init for Lock1 failed: %u\n", ret);
        return 0;
    }
    if (0 != pthread_mutex_init(&Lock2, NULL)) {
        printf("Mutex init for Lock2 failed: %u\n", ret);
        return 0;
    }

    /* Create the thread ids */
    pthread_t tids[MAX_THREADS] = { 0 };

    /* Create the first thread, give it a couple seconds to run */
    if (0 == pthread_create(&tids[0], NULL, thread1_func, NULL)) {
        printf("Thread 1 created, sleeping for a couple seconds to allow it to run before creating Thread2\n");
        sleep(2);
    } else {
        printf("Thread 1 create failed: ret=%u\n", ret);
    }
    
    /* Create the second thread */
    if (0 == pthread_create(&tids[1], NULL, thread2_func, NULL)) {
        printf("Thread 2 created\n");
    } else {
        printf("Thread 2 create failed: ret=%u\n", ret);
    }

    /*
     * Joining allows main to wait for the thread to complete.
     * Without a join, main may exit before the thread gets a chance
     * to run
     */
    for (i = 0; i < MAX_THREADS; i++) {
        pthread_join(tids[i], NULL);
    }

    /* finally, destroy the mutexes */
    pthread_mutex_destroy(&Lock1);
    pthread_mutex_destroy(&Lock2);

    return 0;
}
