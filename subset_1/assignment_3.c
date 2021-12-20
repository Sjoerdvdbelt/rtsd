#include <stdio.h>
#include <pthread.h> 
#include <signal.h>
#include <time.h>

#define LOOPS 1000

void * hello(void *t_result) {
    struct timespec t_start;
    struct timespec t_end;
    sigset_t set; 
    int sig;

    sigemptyset(&set); 
    sigaddset(&set, SIGUSR1);

    clock_gettime(CLOCK_MONOTONIC, &t_start);

    for (int i=0; i<LOOPS; i++) {
        int x = 100000;
        int y = x^2;
        int z = y % x;
        sigwait(&set, &sig);
    }

    clock_gettime(CLOCK_MONOTONIC, &t_end);

    (*(struct timespec*)t_result).tv_nsec = t_end.tv_nsec - t_start.tv_nsec;
    (*(struct timespec*)t_result).tv_sec = t_end.tv_sec - t_start.tv_sec;
    pthread_exit(NULL);
}

int main() {

    struct sigevent event;
    struct itimerspec itime;
    timer_t timer_id;

    event.sigev_notify = SIGEV_SIGNAL;
    event.sigev_signo = SIGUSR1;

    sigset_t set; 
    sigemptyset(&set); 
    sigaddset(&set, SIGUSR1);
    pthread_sigmask(SIG_BLOCK, &set, NULL); 

    timer_create(CLOCK_MONOTONIC, &event, &timer_id);

    itime.it_value.tv_sec = 0;
    itime.it_value.tv_nsec = 1000000; 
    itime.it_interval.tv_sec = 0;
    itime.it_interval.tv_nsec = 1000000; 
    timer_settime(timer_id, 0, &itime, NULL);

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