#include <stdio.h>
#include <pthread.h> 
#include <time.h>

#define LOOPS 1000

void * hello(void *t_result) {
    struct timespec t_delay;
    struct timespec t_start;
    struct timespec t_end;
    t_delay.tv_nsec = 1000000; // 1000000 ns = 1 ms

    clock_gettime(CLOCK_MONOTONIC, &t_start);

    for(int i=0; i<LOOPS; i++) {
        int x = 100000;
        int y = x^2;
        int z = y % x;
        nanosleep(&t_delay, &t_delay);
    }

    clock_gettime(CLOCK_MONOTONIC, &t_end);

    (*(struct timespec*)t_result).tv_nsec = t_end.tv_nsec - t_start.tv_nsec;
    (*(struct timespec*)t_result).tv_sec = t_end.tv_sec - t_start.tv_sec;
    pthread_exit(NULL);
}

int main() {

    struct timespec t_results;
    pthread_t thread_id;
    pthread_create(&thread_id, NULL, hello, (void*)&t_results);
    pthread_join(thread_id,NULL);
    long delta_ns = (t_results.tv_sec * 1000000000) + t_results.tv_nsec;

    puts("Thread returned");
    printf("Seconds: %i, ns: %ld\n", (int) t_results.tv_sec, t_results.tv_nsec);
    printf("Total ns: %ld\n", delta_ns);

    return 0;
}