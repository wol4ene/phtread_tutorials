#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

/***************************************
 *  Tutorial for a pthread_cond
 *
 **************************************/

/* shared variable to be used by all threads */
int shared = 0;

/* mutex all threads will use to access shared var */
pthread_mutex_t lock;

/* condition variable all threads will use */
pthread_cond_t cond;

/* value for 'shared' that is the waiting condition */
#define MAX_VAL 15

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
 * Thread1 - will wait on a condition variable to be satisfied
 *           before proceeding with execution
 */
void*
thread1_func(void *arg) 
{
    tinfo_t *tmp = (tinfo_t*)arg;

    printf("Thread %u - going to wait for shared data to be >= %u\n", 
            tmp->thread_num, MAX_VAL);

    /* Standard procedure for waiting on a condition variable
     *   First lock the mutex
     *   Check the condition in a while loop to guard against spurious wakeups
     *   Call the cond_wait
     *   When signalled, this thread will wake up with lock held
     */
    pthread_mutex_lock(&lock);
    while (shared < MAX_VAL) {
        printf("T1: shared:%u not >= MAX:%u, waiting\n", shared, MAX_VAL);
        pthread_cond_wait(&cond, &lock);
        printf("T1: woken up - need to recheck condition\n");
    }

    printf("T1: Woken up - condition met! Shared data now: %u\n", shared);
    pthread_mutex_unlock(&lock);

    pthread_exit(NULL);
}

/**
 * Thread2 - simulate slowly adding to the shared data until MAX_VAL
 *           is hit, then signal the other thread waiting on it
 */
void*
thread2_func(void *arg) 
{
    tinfo_t *tmp = (tinfo_t*)arg;
    int done = 0;

    printf("Thread %u - going to slowly inc shared data to be >= %u\n", 
            tmp->thread_num, MAX_VAL);

    while (0 == done) {
        int add = rand() % 7;
        printf("T2: Adding %u to shared data\n", add);

        /* Starndard procedure for signalling other threads
         *   Do interesting thing to shared data
         *   Check if condition has been met
         *     If so - unlock the lock and signal 
         */
        pthread_mutex_lock(&lock);
        shared += add;
        if (shared > MAX_VAL) {
            printf("T2: Finally, shared:%u eclipes MAX:%u, signal other thread\n", shared, MAX_VAL);
            pthread_mutex_unlock(&lock);
            pthread_cond_signal(&cond);
            done = 1;
        } else {
            printf("T2: shared:%u does not eclipse MAX:%u, sleep 2 and keep adding\n", shared, MAX_VAL);
            pthread_mutex_unlock(&lock);
            sleep(2);
        }
    }

    pthread_exit(NULL);
}

#define MAX_THREADS 2
int main()
{
    int ret = 0;
    int i = 0;

    printf("Shared data initially starts at %u, MAX_VAL=%u\n",
            shared, MAX_VAL);

    /* init the mutex */
    ret = pthread_mutex_init(&lock, NULL);
    if (0 != ret) {
        printf("Mutex init failed: %u\n", ret);
        return 0;
    }
    /* init the cond var */
    ret = pthread_cond_init(&cond, NULL);
    if (0 != ret) {
        printf("Cond var init failed: %u\n", ret);
        return 0;
    }       

    /* Create the args we will pass to each thread */
    tinfo_t thread_args[MAX_THREADS];
    for (i = 0; i < MAX_THREADS; i++) {
        thread_args[i].thread_num = i+1;
        sprintf(thread_args[i].buf, "Thread Number: %u", i);
    }

    /* Create the thread ids */
    pthread_t tids[MAX_THREADS] = { 0 };

    /* Create the first thread, give it a couple seconds to run */
    if (0 == pthread_create(&tids[0], NULL, thread1_func, (void*)&thread_args[0])) {
        printf("T1 created, sleeping for a couple seconds to allow it to run before creating Thread2\n");
        sleep(2);
    } else {
        printf("T1 create failed: ret=%u\n", ret);
    }
    
    /* Create the second thread */
    if (0 == pthread_create(&tids[1], NULL, thread2_func, (void*)&thread_args[1])) {
        printf("T2 created\n");
    } else {
        printf("T2 create failed: ret=%u\n", ret);
    }

    /*
     * Joining allows main to wait for the thread to complete.
     * Without a join, main may exit before the thread gets a chance
     * to run
     */
    for (i = 0; i < MAX_THREADS; i++) {
        pthread_join(tids[i], NULL);
    }

    /* finally, destroy the mutex and cond var */
    pthread_mutex_destroy(&lock);
    pthread_cond_destroy(&cond);

    return 0;
}
