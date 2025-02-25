/**
 * @file irq.c
 * @brief Simulates hardware interrupts using the lightweight threading library.
 *
 * This example demonstrates how to use the threading library to simulate hardware interrupts.
 * A dedicated thread is created that runs an infinite loop. Every second, it randomly selects
 * one of several interrupt service routines (ISRs) corresponding to different hardware events:
 * keyboard, mouse, timer, and audio. The ISR is then invoked to handle the simulated interrupt.
 */

/**
 * In a computer, an interrupt request (or IRQ) is a hardware signal sent to the processor 
 * that temporarily stops a running program and allows a special program, an interrupt handler, 
 * to run instead. Hardware interrupts are used to handle events such as receiving data from 
 * a modem or network card, key presses, or mouse movements
 * 
 * Source: https://en.wikipedia.org/wiki/Interrupt_request
 * */

#include <stdio.h>
#include <stdlib.h>
#include "qthread.h"
#include <unistd.h>
#include <time.h>

#define NUM_IRQ 4

/**
 * @brief Enum representing hardware interrupt types.
 */
enum HardwareIRQ {
    KEYBOARD_INTERRUPT = 0, /**< Keyboard interrupt identifier */
    MOUSE_INTERRUPT,        /**< Mouse interrupt identifier */
    TIMER_INTERRUPT,        /**< Timer interrupt identifier */
    AUDIO_INTERRUPT         /**< Audio interrupt identifier */
};

/**
 * @brief Interrupt Service Routine for the keyboard interrupt.
 *
 * This function handles the keyboard interrupt by printing a message with the IRQ number.
 *
 * @param irqnumber The IRQ number associated with the keyboard interrupt.
 */
void ISR_KEYBOARD (int irqnumber) {
    printf("Handling interrupt: KEYBOARD_INTERRUPT -> IRQ Number %d\n", irqnumber);
}

/**
 * @brief Interrupt Service Routine for the mouse interrupt.
 *
 * This function handles the mouse interrupt by printing a message with the IRQ number.
 *
 * @param irqnumber The IRQ number associated with the mouse interrupt.
 */
void ISR_MOUSE (int irqnumber) {
    printf("Handling interrupt: MOUSE_INTERRUPT -> IRQ Number %d\n", irqnumber);
}

/**
 * @brief Interrupt Service Routine for the timer interrupt.
 *
 * This function handles the timer interrupt by printing a message with the IRQ number.
 *
 * @param irqnumber The IRQ number associated with the timer interrupt.
 */
void ISR_TIMER (int irqnumber) {
    printf("Handling interrupt: TIMER_INTERRUPT -> IRQ Number %d\n", irqnumber);
}

/**
 * @brief Interrupt Service Routine for the audio interrupt.
 *
 * This function handles the audio interrupt by printing a message with the IRQ number.
 *
 * @param irqnumber The IRQ number associated with the audio interrupt.
 */
void ISR_AUDIO (int irqnumber) {
    printf("Handling interrupt: AUDIO_INTERRUPT -> IRQ Number %d\n", irqnumber);
}

/**
 * @brief Array of interrupt service routines (ISRs).
 *
 * This vector associates hardware interrupt identifiers with their corresponding ISRs.
 */
void (*IRS_vector[NUM_IRQ]) (int) = {
    ISR_KEYBOARD,
    ISR_MOUSE,
    ISR_TIMER,
    ISR_AUDIO
};

/**
 * @brief Thread function that simulates hardware interrupts.
 *
 * This function is executed by a dedicated thread. It initializes the random number generator
 * and enters an infinite loop. Every second, it selects a random interrupt from the available
 * interrupts and calls the corresponding ISR.
 *
 * @param arg Unused parameter.
 */
void interrupt_simulator(void * /* arg */) {
    srand(time(NULL));

    while (1) {
        sleep(1);
        int irq_number = rand() % NUM_IRQ;
        IRS_vector[irq_number](irq_number);
    }
}

/**
 * @brief Main function demonstrating interrupt simulation.
 *
 * The main function creates a thread that runs the interrupt simulator and then starts
 * the thread scheduler. The program will continue to simulate interrupts indefinitely.
 *
 * @return int Returns 0 on successful execution.
 */
int main(void) {
    thread_t *thread;

    if (qthread_create(&thread, interrupt_simulator, NULL)) {
        fprintf(stderr, "Error creating thread\n");
        exit(EXIT_FAILURE);
    }

    qscheduler();

    return 0;
}

