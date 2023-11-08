/**
 * Tony Givargis
 * Copyright (C), 2023
 * University of California, Irvine
 *
 * CS 238P - Operating Systems
 * scm.c
 */

#define _GNU_SOURCE

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>
#include "scm.h"

/**
 * Needs:
 *   fstat()
 *   S_ISREG()
 *   open()
 *   close()
 *   sbrk()
 *   mmap()
 *   munmap()
 *   msync()
 */

/* research the above Needed API and design accordingly */

#define VIRT_ADDR 0x600000000000


struct scm {
    int fd; 
    struct {
        size_t utilized;
        size_t capacity;
    } size;
    void *addr;
};




struct scm *file_size(const char *pathname) {
    struct stat st;
    int fd;
    struct scm *scm;

    assert(pathname);

    if (!(scm = malloc(sizeof(struct scm)))) {
        return NULL;
    }
    memset(scm, 0, sizeof(struct scm));

    if ((fd = open(pathname, O_RDWR)) == -1) {
        free(scm);
        return NULL;
    }

    if (fstat(fd, &st) == -1) {
        free(scm);
        close(fd);
        return NULL;
    }

    if (!S_ISREG(st.st_mode)) {
        free(scm);
        close(fd);
        return NULL;
    }

    scm->fd = fd;
    scm->size.utilized = 0;
    scm->size.capacity = st.st_size;

    return scm;
}

struct scm *scm_open(const char *pathname, int truncate) {

    struct scm *scm = file_size(pathname);
    if (!scm) {
        return NULL;
    }

/*
if (sbrk(scm->size.capacity) == (void *) -1) {
        close(scm->fd);
        free(scm);
        return NULL;
    }
*/    

    if ((scm->addr = mmap((void *) VIRT_ADDR, scm->size.capacity, PROT_READ | PROT_WRITE, MAP_FIXED | MAP_SHARED,
                          scm->fd, 0)) == MAP_FAILED) {
        close(scm->fd);
        free(scm);
        return NULL;
    }

    if (truncate) {
        if (ftruncate(scm->fd, scm->size.capacity) == -1) {
            close(scm->fd);
            free(scm);
            return NULL;
        }
        scm->size.utilized = 0;
    } else {
        scm->size.utilized = (size_t) *(size_t *) scm->addr;
    }
    scm->addr = (char *) scm->addr + sizeof(size_t);
    printf("scm->addr after: %p\n", scm->addr);

    return scm;
}

void scm_close(struct scm *scm) {
    if (scm) {
        msync((char *) VIRT_ADDR, scm->size.capacity, MS_SYNC);
        munmap((char *) VIRT_ADDR, scm->size.capacity);
        close(scm->fd);
        memset(scm, 0, sizeof(struct scm));
    }
    free(scm);
}

void *scm_malloc(struct scm *scm, size_t n) {
    size_t mallocd_n;
    short is_alloc;
    void *p = (char *) scm->addr + scm->size.utilized;
    /* to store flag and n */
    size_t size = sizeof(short) + sizeof(size_t) + n;
    if (scm->size.utilized + size > scm->size.capacity) {
        /* go back to beginning */
        for(p = (char *) scm->addr; (char *)p < ((char *) scm->addr)+scm->size.capacity - sizeof(short) - sizeof(size_t)-sizeof(size_t); ){
            is_alloc = *(short *)p;
            p = (char *) p + sizeof(short);
            mallocd_n = *(size_t *) p;
            p = (char *) p + sizeof(size_t);
            if(!is_alloc){
                /* check if size available is  greater than or equal to size requested */
                if(mallocd_n >= n){
                    /* we can allocate */
                    /* update flag */
                    *(short *) ((char *) p - sizeof(short) - sizeof(size_t)) = 1;
                    return p;
                }
            }
            /* skip to next offset */
            p  = (char *) p + mallocd_n;
        }
        return NULL;
    }
    *(short *) p = 1;
    p = (char *) p + sizeof(short);
    *(size_t *) p = n;
    p = (char *) p + sizeof(size_t);
    scm->size.utilized += size;
    *(size_t *) ((char *) scm->addr - sizeof(size_t)) = scm->size.utilized;
    return p;
}

char *scm_strdup(struct scm *scm, const char *s) {
    size_t n = strlen(s) + 1;
    char *p = scm_malloc(scm, n);
    if (!p) {
        return NULL;
    }
    memcpy(p, s, n);
    return p;
}

void scm_free(struct scm *scm, void *p) {
    if((char *) p < (char *) scm->addr || (char *) p > (char *)scm->addr + scm->size.capacity){
        perror("out of valid range");
    }
    *(short *) ((char *) p - sizeof(short) - sizeof(size_t)) = 0;
}

size_t scm_utilized(const struct scm *scm) {
    return scm->size.utilized;
}

size_t scm_capacity(const struct scm *scm) {
    return scm->size.capacity;
}

void *scm_mbase(struct scm *scm) {
    return (char *) scm->addr + sizeof(short) + sizeof(size_t);
}