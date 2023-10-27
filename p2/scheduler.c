/**
 * Tony Givargis
 * Copyright (C), 2023
 * University of California, Irvine
 *
 * CS 238P - Operating Systems
 * scheduler.c
 */

#undef _FORTIFY_SOURCE

#include <unistd.h>
#include <signal.h>
#include <setjmp.h>
#include "system.h"
#include "scheduler.h"


/**
 * Needs:
 *   setjmp()
 *   longjmp()
 */

/* research the above Needed API and design accordingly */

/* define static state */
static struct{
    struct thread *head; /* linked list of threads */
    struct thread *curr_thread; /* currently executing thread */
    jmp_buf ctx; /* scheduler context */
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
    struct thread* next;
}thread;

struct thread* thread_create(scheduler_fnc_t fnc, void *arg){
    struct thread* new_thread = (struct thread *)malloc(sizeof(struct thread));
    size_t page_sz = page_size();
    new_thread->stack.memory_ = (void*)malloc(4*page_sz);
    if (!new_thread->stack.memory_) {
        perror("malloc");
        return NULL; /* error in memory allocation */
    }
    new_thread->stack.memory = memory_align(new_thread->stack.memory_, page_sz);
    new_thread->status = STATUS_;
    new_thread->fnc = fnc;
    new_thread->arg = arg;
    new_thread->next = NULL;
    return new_thread;
 }

struct thread* thread_candidate(void){
    struct thread* current_ref;
    struct thread* current;
    if(state.curr_thread == NULL){
        current = state.head;
        current_ref = state.head;
    }
    else{
        current = state.curr_thread->next;
        current_ref = state.curr_thread->next;
    }
    do{
        if(current==NULL){
            current = state.head;
        }
        if (current->status == STATUS_ || current->status == STATUS_SLEEPING) {
            return current;
        }
        current = current->next;
 
    }while (current != current_ref);
    return NULL;

}

void schedule(void){
        struct thread *thread = thread_candidate();
        if (thread == NULL) {
            /* all threads completed execution */
            return;
        } else {
                size_t page_sz = page_size();
                uint64_t rsp;
                state.curr_thread = thread;
                if (state.curr_thread->status == STATUS_) {
                    state.curr_thread->status = STATUS_RUNNING;
                    rsp = (uint64_t)thread->stack.memory + 3*page_sz;
                    __asm__ volatile("mov %[rs], %%rsp \n" : [rs] "+r"(rsp)::);
                    (*state.curr_thread->fnc)(state.curr_thread->arg);
                    state.curr_thread->status = STATUS_TERMINATED;
                    longjmp(state.ctx, 1);

                } else if(state.curr_thread->status == STATUS_SLEEPING) {
                    state.curr_thread->status = STATUS_RUNNING;
                    longjmp(state.curr_thread->ctx, 1); 
                }
    }
}

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
        current = current->next;
        free(temp->stack.memory_);
        free(temp);
    }
    state.head = NULL;
    state.curr_thread = NULL;
}

int scheduler_create(scheduler_fnc_t fnc, void *arg ){
    struct thread *new_thread;
    new_thread = thread_create(fnc, arg);
    if(!new_thread){
        return -1;
    }
    new_thread->next = state.head;
    state.head = new_thread;
    return 0; /* success */
}

void signal_handler(int signum) {
  if (signum == SIGALRM) {
    printf("Signal Handler was Triggered\n");
    scheduler_yield();
    alarm(0);
  }
}

void scheduler_execute(void){
    setjmp(state.ctx);
    signal(SIGALRM, signal_handler);
    alarm(1); 
    schedule();
    printf("Scheduler has completed\n");
    destroy();
}

void scheduler_yield(void){
   int val = setjmp(state.curr_thread->ctx);
   if(val==0){
        state.curr_thread->status = STATUS_SLEEPING;
        longjmp(state.ctx, 1);
   }
   else{
    return;
   }

}


