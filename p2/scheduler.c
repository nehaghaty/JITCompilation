/**
 * Tony Givargis
 * Copyright (C), 2023
 * University of California, Irvine
 *
 * CS 238P - Operating Systems
 * scheduler.c
 */

#include <unistd.h>
#include <signal.h>
#include <setjmp.h>
#include "system.h"
#include "scheduler.h"
#undef _FORTIFY_SOURCE
#define NDEBUG
#define STACK_SIZE (1024 * 1024)

/* define static state */
static struct{
    /* linked list of threads */
    struct thread * head;
    /* currently executing/running thread, to find running thread in O(1) time */
    struct thread* thread;
    /* mark a common place for scheduler */
    jmp_buf ctx;
} state;

/* Thread Data Structure */
struct thread {
	jmp_buf ctx;
	enum {
	  STATUS_,
  	  STATUS_RUNNING,
  	  STATUS_SLEEPING,
  	  STATUS_TERMINATED
  	}status;
	struct{
		void* memory_; /* returned by malloc, this is what needs to be freed */
		void* memory; /* the one we will use, region of heap space */
	} stack;
    scheduler_fnc_t fnc;
    void *arg;
    struct thread* link; /* linked ptr to next thread */
}thread;

/* struct thread* thread_create(){
     struct thread* my_thread = malloc(sizeof(thread));
     my_thread->stack.memory_ = malloc(1048576);
     size_t p_s = page_size();
     my_thread->stack.memory = memory_align(p_s, my_thread->stack.memory_);
     my_thread->link = NULL;
     my_thread->status = STATUS_;
 }*/

struct thread* thread_candidate(void){
    /*traverse list, find valid states: state_, state_sleeping
    return next thread
    return null if nothing runnable: terminating condition, else return thread object */
    struct thread *current = state.head;
    while (current != NULL) {
        if (current->status == STATUS_ || current->status == STATUS_SLEEPING) {
            return current;
        }
        current = current->link;
    }
    return NULL;

}

void schedule(void){
    /*
        create thread object = thread candidate
            if thread candidate is null return 
            else
                first time executing
                    if thread not initialized
                    initialize stack <-- assembly
                    call function instead of longjmp to state
                not first time
                    longjmp(thread->ctx);
    */

        struct thread *thread = thread_candidate();
        if (!thread) {
            /* all threads completed execution */
            return;
        } else {
                state.thread = thread;
                if (thread->status == STATUS_) {
                    uint64_t rsp = (uint64_t)thread->stack.memory + STACK_SIZE; 
                    /* Simulated assembly code */ 
                    __asm__ volatile("mov %[rs], %%rsp \n" : [rs] "+r"(rsp)::);
                    thread->status = STATUS_RUNNING;
                    thread->fnc(thread->arg);
                } else {
                    thread->status = STATUS_RUNNING;
                    longjmp(thread->ctx, 1);
                }
    }
}



/*dstroy call at the end */
void destroy(void){
    /*
    traverse the linkedlist
    find head
    deallocate 
    state.head = NULL;
    state.thread = NULL;
    */
    struct thread* current = state.head;
    while (current != NULL) {
        /* put in function */
        struct thread* temp = current;
        current = current->link;
        free(temp->stack.memory_);
        free(temp);
    }
    state.head = NULL;
    state.thread = NULL;
}

int scheduler_create(scheduler_fnc_t fnc, void *arg ){
    
    /*
    gets the page_size
    memory aligns the page
    allocate thread
    init
        fnc array
        allocate stack
    link it state head
    thread* new_thread = thread_create();
    new_thread->link = state.head;
    state.head = new_thread;
    return 0;
    */
    size_t page_sz = page_size();
    void *stack_memory = malloc(page_sz + STACK_SIZE);
    struct thread *new_thread = (struct thread *)malloc(sizeof(struct thread));

    if (!stack_memory) {
        perror("malloc");
        return -1; /* error in memory allocation */
    }
    if (!new_thread) {
        free(stack_memory);
        return -1; /* error in memory allocation*/
    }
    new_thread->status = STATUS_;
    new_thread->stack.memory_ = stack_memory;
    new_thread->stack.memory = memory_align(stack_memory, page_size());
    new_thread->fnc = fnc;
    new_thread->arg = arg;
    new_thread->link = state.head;
    state.head = new_thread;
    return 0; /* success */


}

void scheduler_execute(void){
    /* checkpoint -> state ctx setjmp(state.ctx)
     schedule() <-- majority of time will be spent here
    destroy() */
    setjmp(state.ctx);
    schedule(); /* majority of time will be spent here */
    destroy();
}

void scheduler_yield(void){
    /* checkpoint-> currently running thread
    two paths 
    return either null
    or returns checkpoint of old thread and makes it sleep
    state.thread -> status = SLEEPING; <-- do this first, in case of only thread
    longjmp(state.ctx) <-- go back to old state 
    */
   /* checkpoint-> currently running thread */
   if(!setjmp(state.thread->ctx)){
        state.thread->status = STATUS_SLEEPING;
        longjmp(state.ctx, 1);
   }

}

/**
 * Needs:
 *   setjmp()
 *   longjmp()
 */

/* research the above Needed API and design accordingly */
