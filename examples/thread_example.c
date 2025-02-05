#include "qthread.h"
#include <stdio.h>
#include <stdlib.h>

/**
 * @brief Thread function that prints its ID and iteration.
 *
 * This function is executed by each thread created by the threading library.
 * It receives an integer ID via the argument, frees the allocated memory for it,
 * and then performs 5 iterations. In each iteration, it prints its thread ID,
 * its own pointer (using `qthread_self()`), and the current iteration number.
 * After printing, it yields control to the scheduler by calling `qscheduler()`.
 *
 * @param arg Pointer to an integer that represents the thread's ID.
 */
void thread_function(void *arg) {
    int id = *(int *)arg;  // Retrieve the thread's ID.
    free(arg);             // Free the allocated memory for the ID.

    // Perform 5 iterations.
    for (int i = 0; i < 5; i++) {
        printf("Thread %d (self: %p): iteration %d\n", id, qthread_self(), i);
        qscheduler();      // Yield control to the scheduler.
    }
}

/**
 * @brief Main function demonstrating thread creation, execution, and joining.
 *
 * The main function sets a custom stack size for the threads, creates three threads,
 * and passes a unique ID to each. After starting the threads, it waits for their completion
 * using `qthread_join`. Finally, once all threads have finished executing, it prints a message.
 *
 * @return int Returns 0 on successful execution.
 */
int main() {
    // Set the stack size for new threads (128 KB in this example).
    qthread_set_stacksize(128 * 1024);

    // Thread pointers for the three threads.
    thread_t *thread1, *thread2, *thread3;

    // Allocate and assign unique IDs for each thread.
    int *id1 = malloc(sizeof(int));
    int *id2 = malloc(sizeof(int));
    int *id3 = malloc(sizeof(int));
    *id1 = 1;
    *id2 = 2;
    *id3 = 3;

    // Create three threads, each executing thread_function with its corresponding ID.
    qthread_create(&thread1, thread_function, id1);
    qthread_create(&thread2, thread_function, id2);
    qthread_create(&thread3, thread_function, id3);

    // Wait for all threads to finish execution.
    qthread_join(thread1, NULL);
    qthread_join(thread2, NULL);
    qthread_join(thread3, NULL);

    // Indicate that all threads have finished.
    printf("All threads finished.\n");
    return 0;
}