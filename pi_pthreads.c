#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/time.h>     // gethrvtime
/* Number of threads */
#define N_THREADS 96
//2 billion terms -> ~8 digits of precision
#define N_TERMS 2000000000LL
/* Thread args data structure */
struct thread_args{
    long long start_k;// Starting k index
    long long workload;     // Number of elements to process
    double * sum;
};
typedef struct thread_args thread_args_t;
/* Create mutex to protect sum*/
pthread_mutex_t mutexsum;

/* Thread callback */
void * sum_pi(void * args_ptr){
    thread_args_t * t_args = (thread_args_t *)args_ptr;
    long long start_k = t_args->start_k;
    long long workload= t_args->workload;
    long long end_k = start_k+workload;
    double local_sum = 0.0;
    /* Sum terms */
    for(long long k=start_k; k<end_k; ++k){
        local_sum+=(k % 2 ? -1.0 : 1.0) / (2.0 * k + 1.0);
    }
   
    /*Lock a mutex prior to updating shared value sum */
    pthread_mutex_lock (&mutexsum);
    *t_args->sum+=local_sum;

    /*unlock mutex upon updating*/
    pthread_mutex_unlock (&mutexsum);
    
    /* Exit thread */
    pthread_exit(NULL);
}

int main()
{
    /* Initialize Pi */
    double pi_estimate=0.0;
    long long workload = N_TERMS/N_THREADS;
    /*Create pthread objects*/
    pthread_t pi_threads[N_THREADS];
    thread_args_t t_args[N_THREADS];

    /* Begin measuring */
    hrtime_t start, end;
    start = gethrvtime();

    /* Fork: */
    for(int i=0;i<N_THREADS;++i){
        t_args[i].sum = &pi_estimate;
        t_args[i].workload = workload;
        t_args[i].start_k = i*workload;
        /* Create pthread :
        * int pthread_create(pthread_t *thread, const pthread_attr_t *attr,
                          void *(*start_routine) (void *), void *arg); */
        if(pthread_create(&pi_threads[i],NULL,sum_pi,(void *)&t_args[i])){
            fprintf(stderr, "Error creating thread!\n");
            return 1;
        }
    }

    /* Join: */
    for(int i=0;i<N_THREADS;++i){
        /* Wait for threads to finish */
        /* int pthread_join(pthread_t thread, void **value_ptr); */
        if(pthread_join(pi_threads[i], NULL)) {
            fprintf(stderr, "Error joining thread %d\n",i);
            return 2;
        }
    }
    /* Destroy mutex */
    pthread_mutex_destroy(&mutexsum);

    /* Compute terms that were not a multiple of N_THREADS*/
    for(long long  k=N_THREADS*workload;k<N_TERMS;++k){
        pi_estimate+=(k % 2 ? -1.0 : 1.0) / (2 * k + 1);
    }
    /* Multiply by four */
    pi_estimate *= 4.0;
    /* Report time and estimate */
    end = gethrvtime();
    printf("Time: %lld nsec\n",end - start);
    printf("Leibniz Pi estimate with %lld terms: \n %.9lf\n", N_TERMS, pi_estimate);
    

    printf("DONE!\n");
    /* Exit main thread */
    pthread_exit(NULL);
    return 0;
}
