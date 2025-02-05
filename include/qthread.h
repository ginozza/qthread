/**
 * @file qthread.h
 * @brief Custom lightweight threading library using user-level threads.
 *
 * This library provides basic threading functionalities such as thread creation,
 * scheduling, and joining using user-level threads implemented with `ucontext.h`.
 *
 * @author ginozza
 * @date 2025
 */
#ifndef QTHREAD_H
#define QTHREAD_H

#include <ucontext.h>
#include <stddef.h>

/// Default stack size for threads (modifiable with qthread_set_stacksize)
#define DEFAULT_STACK_SIZE (64 * 1024)

/**
 * @enum thread_state
 * @brief Represents the state of a thread.
 */
typedef enum { READY, RUNNING, FINISHED } thread_state;

/**
 * @struct thread
 * @brief Structure representing a user-level thread.
 */
typedef struct thread {
    ucontext_t context; ///< Thread execution context.
    void *stack; ///< Pointer to allocated stack memory.
    thread_state state; ///< Current state of the thread.
    struct thread *next; ///< Pointer to the next thread in the circular list.
    void *retval; ///< Return value for the thread (used by qthread_join).
} thread_t;

// Global circular linked list head for thread management.
extern thread_t *thread_list;

/**
 * @brief Sets the stack size for newly created threads.
 *
 * This function should be called before creating any threads.
 *
 * @param size New stack size in bytes.
 */
void qthread_set_stacksize(size_t size);

void qthread_wrapper(void (*func)(void*), void *arg);

/**
 * @brief Creates a new thread.
 *
 * Initializes a thread with a given function and argument.
 *
 * @param[out] new_thread Pointer to store the created thread (can be NULL).
 * @param[in] start_routine Function to be executed by the thread.
 * @param[in] arg Argument passed to the start_routine function.
 * @return 0 on success, -1 on failure.
 */
int qthread_create(thread_t **new_thread, void(*start_routine)(void *), void *arg);

/**
 * @brief Waits for a thread to complete.
 *
 * Blocks until the specified thread has finished execution.
 * If `retval` is not NULL, stores the thread's return value.
 *
 * @param[in] thread Thread to wait for.
 * @param[out] retval Pointer to store the thread's return value (can be NULL).
 * @return 0 on success, -1 on failure.
 */
int qthread_join(thread_t *thread, void **retval);

/**
 * @brief Terminates the current thread.
 *
 * Marks the current thread as finished and triggers the scheduler.
 * Optionally stores a return value for retrieval by qthread_join.
 *
 * @param[in] value Return value to be stored (can be NULL).
 */
void qthread_exit(void *value);

/**
 * @brief Switches execution to the next available thread.
 */
void qscheduler();

/**
 * @brief Retrieves the currently running thread.
 *
 * @return Pointer to the current thread.
 */
thread_t *qthread_self();

/**
 * @brief Initializes the scheduler by saving the main context as a thread.abort
 * 
 * This function must be called from main() before creating other threads.
 */
void qthread_init();

#endif // QTHREAD_H
