# qthread - Lightweight Threading Library in C

A lightweight cooperative threading library implementing user-level threads with round-robin scheduling.

## Features
- Cooperative thread management.
- Round-robin scheduling.
- Custom stack size configuration.
- Thread creation and joining.
- Context switching via manual yielding.

## Requirements 
- C compiler (gcc/clang).
- POSIX-compliant system with ucontext functions (part of glibc).
- GNU Make (build automation).

## Compilation
```bash
# Clone the repository
git clone https://github.com/yourusername/qthreads.git
cd qthreads

# Build project (library + example)
make

# Run the example
./build/thread_example
```

# Learning qthread

## I. API Documentation
```c
// Configure stack size for new threads, must be called before thread creation.
void qthread_set_stacksize(size_t size);

// Create a new thread.
int qthread_create(thread_t **thread, void (*func)(void *), void *arg);

// Yield execution to next available thread.
void qscheduler(void);

// Terminate current thread.
void qthread_exit(void *retval);

// Wait for a thread to finish execution.
int qthread_join(thread_t *thread, void **retval);

// Get current thread handle.
thread_t *qthread_self(void);
```

## II. Examples
Sample usage implementations are available in the `examples/` directory.

## Structure
```bash
qthreads/
├── include/
│   └── qthread.h          # Public API header
├── src/
│   └── qthread.c          # Library implementation
├── examples/
│   └── main.c             # Demonstration program
├── build/                 # Build artifacts (created during compilation)
├── Makefile               # Build configuration
└── README.md              # This documentation
``` 

**@ginozza**. This project is part of an independent case study for learning the Operating Systems course at the Universidad del Magdalena.