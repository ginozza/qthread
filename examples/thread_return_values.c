#include "qthread.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

/**
 * @brief Worker thread function.
 *
 * This function is executed by each worker thread. It receives an integer ID as an argument,
 * frees the allocated memory for the ID, and then performs one of two actions:
 *
 * - For the worker with ID 4, it exits with a special error value (0xDEADBEEF).
 * - For all other workers, it prints a "working" message three times, calls the scheduler
 *   after each message to yield control, computes a result (ID * 100), and exits with that result.
 *
 * @param arg A pointer to an integer representing the worker ID.
 */
void worker_thread(void *arg) {
    int id = *(int *)arg;
    free(arg);
    
    if (id == 4) {
        // Worker 4: exit with a special error value.
        qthread_exit((void *)(intptr_t)0xDEADBEEF);
    } else {
        // Other workers: perform work and return a computed result.
        for (int i = 0; i < 3; i++) {
            printf("Worker %d: working...\n", id);
            qscheduler();
        }
        int *result = malloc(sizeof(int));
        *result = id * 100;
        qthread_exit(result);
    }
}

/**
 * @brief Main function demonstrating the custom threading library.
 *
 * This function performs the following steps:
 * 1. Incorporates the main thread into the scheduler's thread list.
 * 2. Creates 5 worker threads.
 * 3. Starts the scheduler to execute the worker threads.
 * 4. After all workers have completed, it collects and prints their return values.
 *
 * The main thread is added to the thread list so that the scheduler can return control
 * back to it once all the worker threads finish executing.
 *
 * @return int Returns 0 upon successful execution.
 */
int main() {
    // 1. Incorporate the main thread into the thread list.
    thread_t main_thread;
    if (getcontext(&main_thread.context) == -1) {
        perror("getcontext");
        exit(EXIT_FAILURE);
    }
    main_thread.state = READY;
    main_thread.stack = NULL;          // The main thread uses the standard process stack.
    main_thread.next = &main_thread;     // Initially, it is the only thread in the circular list.
    thread_list = &main_thread;

    // Set a smaller stack size for demonstration purposes.
    qthread_set_stacksize(64 * 1024);

    // 2. Create 5 worker threads.
    thread_t *workers[5];
    for (int i = 0; i < 5; i++) {
        int *id = malloc(sizeof(int));
        *id = i + 1;
        qthread_create(&workers[i], worker_thread, id);
    }

    // 3. Start the scheduler to execute the worker threads.
    // When there are no more READY threads, control will return to the main thread.
    qscheduler();

    // 4. Collect the results from each worker thread.
    for (int i = 0; i < 5; i++) {
        void *retval;
        qthread_join(workers[i], &retval);
        
        if ((intptr_t)retval == 0xDEADBEEF) {
            printf("Thread %d: ERROR\n", i+1);
        } else {
            printf("Thread %d: Result = %d\n", i+1, *(int *)retval);
            free(retval);
        }
    }

    printf("All workers completed\n");
    return 0;
}
