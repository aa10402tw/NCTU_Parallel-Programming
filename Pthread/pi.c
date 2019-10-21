#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

long num_of_trial;
long num_in_circle;
long num_thread;

pthread_mutex_t mutex;

void* toss(void* arg) {
    long num_of_toss = (long)arg;
    long local_num_in_circle = 0;
    int i=0;
    for(i=0; i<num_of_toss; i++) {
        double x, y;
        x = (double)rand()/RAND_MAX*2.0-1.0;//float in range -1 to 1
        y = (double)rand()/RAND_MAX*2.0-1.0;//float in range -1 to 1
        double dist_squared = x*x + y*y;
        if(dist_squared <= 1)
            local_num_in_circle += 1;
    }
    pthread_mutex_lock(&mutex);
    /* Critical Section */
    printf("Local Num %ld", local_num_in_circle);
    num_in_circle += local_num_in_circle;
    /* Critical Section */
    pthread_mutex_unlock(&mutex);
    return NULL;
}

int main(int argc, char const *argv[])
{
    srand ( time ( NULL));
    num_in_circle = 0;
    num_thread = strtoll(argv[1], NULL, 10);
    num_of_trial = strtoll(argv[2], NULL, 10);
    printf("Num of Thread:%ld \t Num of Toss:%ld \n", num_thread, num_of_trial);
    pthread_mutex_init(&mutex, NULL);

    long num_of_trial_per_thread = num_of_trial / num_thread;
    pthread_t* thread_handles = (pthread_t*) malloc (num_thread*sizeof(pthread_t));
    int i=0;

    // Create Threads
    for (i = 0; i < num_thread; i++) {
        pthread_create(&thread_handles[i], NULL,
            toss, (void*)num_of_trial_per_thread);
    }

    // Join Threads
    for (i = 0; i < num_thread; i++) {
        pthread_join(thread_handles[i], NULL);
    }
    double pi_estimated = (4*num_in_circle) / ((double)num_of_trial);
    printf("Estimated Pi: %lf\n", pi_estimated);

    pthread_mutex_destroy(&mutex);
    free(thread_handles);
    return 0;
}