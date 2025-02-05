/*
 * @file qthread.c 
 * @brief Implementation of the lightweight threading library.
 *
 * This file provides the implementations of user-level threads, including
 * creation, scheduling, and termination.
 */
#include "../include/qthread.h"
#include <stdlib.h>
#include <stdio.h>

/// Global stack size variable (modifiable via qthread_set_stacksize).
static size_t stack_size = DEFAULT_STACK_SIZE;

/// Head of the circular thread list.
thread_t *thread_list = NULL;

/// Currently running thread.
thread_t *current = NULL;

/**
 * @brief Sets the stack size for new threads.
 * 
 * Must be called before creating threads to take effect.
 *
 * @param size New stack size in bytes.
 */
void qthread_set_stacksize(size_t size) {
    stack_size = size;
}

/**
 * @brief Returns the currently running thread.
 *
 * @return Pointer to the current thread.
 */
thread_t *qthread_self() {
    return current;
}

/**
 * @brief Schedules the next available READY thread.
 */
void qscheduler() {
    if (!thread_list) return; // No threads to schedule

    thread_t *t = NULL;

    if (current == NULL) {
        // Initial call: search all threads for the first READY one
        t = thread_list;
        do {
            if (t->state == READY) {
                break;
            }
            t = t->next;
        } while (t != thread_list);
    } else {
        // Normal case: search from current->next
        t = current->next;
        while (t != current) {
            if (t->state == READY) {
                break;
            }
            t = t->next;
        }

        // After loop, t is current or a READY thread
        if (t->state != READY) {
            // No other READY thread found; check current
            if (current->state == READY) {
                t = current;
            } else {
                // All threads are not READY
                return;
            }
        }
    }

    // Ensure we have a READY thread
    if (t->state != READY) return;

    // Perform context switch if necessary
    if (current == NULL) {
        // Initial switch: no previous context to save
        current = t;
        setcontext(&current->context);
    } else if (t != current) {
        thread_t *prev = current;
        current = t;
        swapcontext(&prev->context, &current->context);
    }
}

/**
 * @brief Terminates the current thread and schedules another.
 *
 * Marks the current thread as finished and optionally stores a return value.
 *
 * @param[in] value Return value to be stored (can be NULL).
 */
void qthread_exit(void *value) {
    current->retval = value; // Store return value
    current->state = FINISHED; // Mark thread as finished
    qscheduler(); // Schedule the next thread
}

/**
 * @brief Wrapper function for thread execution.
 *
 * Calls the given function and automatically exits upon completion.
 *
 * @param func Function to execute.
 * @param arg Argument to pass to the function.
 */
void qthread_wrapper(void (*func)(void*), void *arg) {
    func(arg); // Execute the function
    qthread_exit(NULL); // Automatically exit after completion
}

/**
 * @brief Creates a new thread.
 *
 * Allocates memory, initializes the context, and adds the thread to the list.
 *
 * @param[out] new_thread Pointer to store the created thread (can be NULL).
 * @param[in] start_routine Function executed by the thread.
 * @param[in] arg Argument passed to the function.
 * @return 0 on success, -1 on failure.
 */
int qthread_create(thread_t **new_thread, void (*start_routine)(void *), void *args) {
    thread_t *t = malloc(sizeof(thread_t));
    if (!t) return -1;

    t->stack = malloc(stack_size);
    if (!t->stack) {
        free(t);
        return -1;
    }

    t->state = READY;

    if (getcontext(&t->context) == -1) {
        free(t->stack);
        free(t);
        return -1;
    }
    t->context.uc_stack.ss_sp = t->stack;
    t->context.uc_stack.ss_size = stack_size;
    t->context.uc_link = NULL;
    makecontext(&t->context, (void (*)()) qthread_wrapper, 2, start_routine, args);

    // Insert into circular list
    if (!thread_list) {
        thread_list = t;
        t->next = t;
        // Do not set current here; it will be handled by scheduler
    } else {
        thread_t *temp = thread_list;
        while (temp->next != thread_list)
            temp = temp->next;
        temp->next = t;
        t->next = thread_list;
    }

    if (new_thread)
        *new_thread = t;

    return 0;
}

/**
 * @brief Waits for a thread to finish.
 *
 * Blocks execution until the specified thread completes.
 *
 * @param[in] thread Thread to wait for.
 * @param[out] retval Pointer to store the thread's return value (can be NULL).
 * @return 0 on success, -1 on failure.
 */
int qthread_join(thread_t *thread, void **retval) {
    while (thread->state != FINISHED) {
        qscheduler(); // Wait until the thread finishes
    }

    if (retval) *retval = thread->retval; // Store return value if requested
    
    if (thread_list) {
        thread_t *prev = thread_list;
        thread_t *curr = thread_list->next;

        if (prev == curr && prev == thread) {
            thread_list = NULL;
        } else {
            while (curr != thread && curr != thread_list) {
                prev = curr;
                curr = curr->next;
            }

            if (curr == thread) {
                prev->next = curr->next;
                if (thread_list == curr) {
                    thread_list = prev->next;
                }
            }
        }
    }

    free(thread->stack); // Free allocated stack
    free(thread); // Free thread structure

    return 0; // Success
}
