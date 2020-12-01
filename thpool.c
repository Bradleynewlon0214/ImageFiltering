#include "thpool.h"
#include <pthread.h>
#include <stdlib.h>
#include "iplib2New.h"
#include <string.h>


struct tpool_work {
    thread_func_t       func;
    int                 index;
    unsigned char       *values;
    int                 *mask_one;
    int                 *mask_two;
    bool                both;
    struct tpool_work   *next;
};
typedef struct tpool_work tpool_work_t;


struct tpool {
    tpool_work_t    *work_first;
    tpool_work_t    *work_last;
    pthread_mutex_t  work_mutex;
    pthread_cond_t   work_cond;
    pthread_cond_t   working_cond;
    size_t           working_cnt;
    size_t           thread_cnt;
    bool             stop;
};

/*
* This function is a helper function that accepts a threads to-be work function, it's image index, a pointer to an array of sublock values, two image masks, and a boolean
* that indicates whether or not the work function will have to calculate an edge response using both image masks or just one. The function will then return the work struct   
*/
static tpool_work_t *tpool_work_create(thread_func_t func, int index, unsigned char *values, int *mask_one, int *mask_two, bool both){
    tpool_work_t *work;

    if (func == NULL)
        return NULL;

    work       = malloc(sizeof(*work));
    work->func = func;
    work->index = index;
    work->values = values;
    work->mask_one = mask_one;
    work->mask_two = mask_two;
    work->both = both;
    work->next = NULL;
    return work;
}

/*
* This function accepts 
*/
static void tpool_work_destroy(tpool_work_t *work){
    if (work == NULL)
        return;
    free(work);
}

/*
* This function accepts a pointer to the thread pool and returns the the first available work in the work queue.
* If not, it will return null.
*/
static tpool_work_t *tpool_work_get(tpool_t *tm){
    tpool_work_t *work;

    if (tm == NULL)
        return NULL;

    work = tm->work_first;
    if (work == NULL)
        return NULL;

    if (work->next == NULL) {
        tm->work_first = NULL;
        tm->work_last  = NULL;
    } else {
        tm->work_first = work->next;
    }

    return work;
}

/*
* This function monitors the work queue and processes work. First the function will lock the queue then it will check if any work is availble then the function
* will wait in the condidtion until it gets the signal to check for work again. Next, if the pool has been signaled to stop, it will break out of the loop
* and release the mutex lock. The function will then pull work from the queue, increment the number of threads that the pool has working, and release the mutext lock.
* As long as the work is not null the work will be sent to the work function then the work will be freed from memory when it it complete.
* After the work is complete, the function will grab the mutex lock again, decrement the number of threads working in the thread pool, then as long as there is more work
* a signal will be sent to wake up any sleeping threads so they can pull work.
*/
static void *tpool_worker(void *arg){
    tpool_t      *tm = arg;
    tpool_work_t *work;

    while (1) {
        pthread_mutex_lock(&(tm->work_mutex));
        
        while (tm->work_first == NULL && !tm->stop)
            pthread_cond_wait(&(tm->work_cond), &(tm->work_mutex));

        if (tm->stop)
            break;

        work = tpool_work_get(tm);
        tm->working_cnt++;
        pthread_mutex_unlock(&(tm->work_mutex));

        if (work != NULL) {
            work->func(work->index, work->values, work->mask_one, work->mask_two, work->both);
            tpool_work_destroy(work);
        }

        pthread_mutex_lock(&(tm->work_mutex));
        tm->working_cnt--;
        if (!tm->stop && tm->working_cnt == 0 && tm->work_first == NULL)
            pthread_cond_signal(&(tm->working_cond));
        pthread_mutex_unlock(&(tm->work_mutex));
    }

    tm->thread_cnt--;
    pthread_cond_signal(&(tm->working_cond));
    pthread_mutex_unlock(&(tm->work_mutex));
    return NULL;
}

/*
* This function takes the number of desired threads and creates the thread pool. If the desired number of threads is 0 then the thead pool defaults to create two threads.
* The function will initialize the mutex lock, and the condidtional signals then create the desired number of threads, pass them the tpool_worker function which waits on the work queue
* then detatches the threads so they can clean up when they are finished.
*/
tpool_t *tpool_create(size_t num){
    tpool_t   *tm;
    pthread_t  thread;
    size_t     i;

    if (num == 0)
        num = 2;

    tm             = calloc(1, sizeof(*tm));
    tm->thread_cnt = num;

    pthread_mutex_init(&(tm->work_mutex), NULL);
    pthread_cond_init(&(tm->work_cond), NULL);
    pthread_cond_init(&(tm->working_cond), NULL);

    tm->work_first = NULL;
    tm->work_last  = NULL;

    for (i=0; i<num; i++) {
        pthread_create(&thread, NULL, tpool_worker, tm);
        pthread_detach(thread);
    }

    return tm;
}

/*
* This function accepts the pointer to the thread pool, and empties the work queue. The function will then get rid of all the mutex locks and conditional signals.
* If this function is called while there is still work in the queue, the work will be destroyed.
*/
void tpool_destroy(tpool_t *tm){
    tpool_work_t *work;
    tpool_work_t *work2;

    if (tm == NULL)
        return;

    //remove all work from queue
    pthread_mutex_lock(&(tm->work_mutex));
    work = tm->work_first;
    while (work != NULL) {
        work2 = work->next;
        tpool_work_destroy(work);
        work = work2;
    }
    //signal to all remaining waiting threads to stop
    tm->stop = true;
    pthread_cond_broadcast(&(tm->work_cond));
    pthread_mutex_unlock(&(tm->work_mutex));

    tpool_wait(tm); //wait for currently working threads to finish

    pthread_mutex_destroy(&(tm->work_mutex));
    pthread_cond_destroy(&(tm->work_cond));
    pthread_cond_destroy(&(tm->working_cond));

    free(tm);
}

/*
* This function 
*/
bool tpool_add_work(tpool_t *tm, thread_func_t func, int index, unsigned char *values, int *mask_one, int *mask_two, bool both){
    tpool_work_t *work;

    if (tm == NULL)
        return false;

    //create work
    work = tpool_work_create(func, index, values, mask_one, mask_two, both);
    if (work == NULL)
        return false;

    //grab mutex lock and add to work queue
    pthread_mutex_lock(&(tm->work_mutex));
    if (tm->work_first == NULL) {
        tm->work_first = work;
        tm->work_last  = tm->work_first;
    } else {
        tm->work_last->next = work;
        tm->work_last       = work;
    }
    //signal to waiting threads that there is work
    pthread_cond_broadcast(&(tm->work_cond));
    pthread_mutex_unlock(&(tm->work_mutex));

    return true;
}

/*
* This function accepts a pointer to the thread pool and will wait on on all the working threads to finish
*/
void tpool_wait(tpool_t *tm){
    if (tm == NULL)
        return;
    //grab lock so no more work is added
    pthread_mutex_lock(&(tm->work_mutex));
    while (1) {
        //wait for working threads to finish
        if ((!tm->stop && tm->working_cnt != 0) || (tm->stop && tm->thread_cnt != 0)) {
            pthread_cond_wait(&(tm->working_cond), &(tm->work_mutex));
        } else {
            break;
        }
    }
    pthread_mutex_unlock(&(tm->work_mutex));
}