/**
 * Tony Givargis
 * Copyright (C), 2023
 * University of California, Irvine
 *
 * CS 238P - Operating Systems
 * jitc.c
 */

#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <dlfcn.h>
#include "system.h"
#include "jitc.h"

/**
 * Needs:
 *   fork()
 *   execv()
 *   waitpid()
 *   WIFEXITED()
 *   WEXITSTATUS()
 * 
 *   dlopen()
 * 
 *   dlclose()
 * 
 *   dlsym()
 */

/* research the above Needed API and design accordingly */

struct jitc
{
  void *handle;
};

struct jitc *jitc;

int jitc_compile(const char *input, const char *output){
    /* call fork */
    int pid = fork();
    /* char* inp = (char*)input; */
    if ( pid == -1 ) {
        perror("fork failed");
        exit(EXIT_FAILURE);
    }
    else if(pid == 0){
        /* child process */
        char *argv[] = {"gcc", "-fPIC", "-shared", "-o", NULL, NULL, NULL};
        argv[4] = (char*)output;
        argv[5] = (char*)input;

        execv("/usr/bin/gcc", argv);
    }
    else {
        /* parent process */
        int status;
        if(waitpid(pid, &status, 0) != -1){
            int exit_status = WEXITSTATUS(status);
            /* non zero means normal exit */
            if ( WIFEXITED(status)) {
                printf("jitc_compile succcessful. Exit status: %d\n", exit_status);
                return 0;
            }
            else{
                printf("jitc_compile failed. Abnormal exit status: %d\n", exit_status);
            }
        }
    }

    exit(EXIT_FAILURE);
}

struct jitc *jitc_open(const char *pathname){
    /* search if typecast */ 
    jitc = malloc(sizeof(struct jitc));
    if(jitc==NULL){
        printf("jitc_open failed. Failed to allocate memory with malloc.\n");
        return NULL;
    }
    jitc->handle = dlopen (pathname, RTLD_LAZY|RTLD_LOCAL);
    if (jitc->handle == NULL) {
        printf("jitc_open failed. Error while loading .so file: %s\n", dlerror());
        return NULL;
    }
    printf("jitc_open successful\n");
    return jitc;
}

long jitc_lookup(struct jitc *jitc, const char *symbol){

    long lookup =  (long) dlsym(jitc->handle, symbol);
    if(lookup){
        printf("jitc_lookup successful\n");
        return lookup;
    }
    else{
        printf("jitc_lookup failed. Error: %s\n", dlerror());
        return 0;
    }

}

/**
 * Unloads a previously loaded dynamically loadable module.
 *
 * jitc: an opaque handle previously obtained by calling jitc_open()
 *
 * Note: jitc may be NULL
 */

void jitc_close(struct jitc *jitc){

    int errorClose = dlclose(jitc->handle);
    if(errorClose){
        printf("jitc_close failed. Error: %s\n", dlerror());
    }
    else{
        printf("jitc_close successful\n");
    }
    /* free malloc */
    free(jitc);
}








