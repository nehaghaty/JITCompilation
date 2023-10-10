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
        char *argv[] = {"gcc", "-O3", "-fPIC", "-shared", "-Werror", "-Wextra", "-o", NULL, NULL, NULL};
        argv[7] = (char*)output;
        argv[8] = (char*)input;

        execv("/usr/bin/gcc", argv);
        exit(0);
    }
    else {
        /* parent process */
        int status;
        waitpid(pid, &status, 0);
        /* non zero means normal exit */
        if ( WIFEXITED(status)) {
            /* jitc compile successful*/
            return WEXITSTATUS(status);
            }

        else{
            
            /*jitc compile failed*/
            return status;
            }
        }
    }

struct jitc *jitc_open(const char *pathname){
    /* search if typecast */ 
    jitc = malloc(sizeof(struct jitc));
    if(jitc==NULL){
        /* malloc failed*/
        return NULL;
    }
    jitc->handle = dlopen (pathname, RTLD_LAZY|RTLD_LOCAL);
    if (jitc->handle == NULL) {
        /* loading .so file failed*/
        return NULL;
    }
    /* jitc open successful*/
    return jitc;
}

long jitc_lookup(struct jitc *jitc, const char *symbol){
    /* dlsym return non zero value on success*/
    return (long) dlsym(jitc->handle, symbol);

}

void jitc_close(struct jitc *jitc){
    /* dlclose returns 0 on success*/
    dlclose(jitc->handle);
    /* free malloc */
    free(jitc);
}








