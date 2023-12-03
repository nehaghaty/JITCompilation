/**
 * Tony Givargis
 * Copyright (C), 2023
 * University of California, Irvine
 *
 * CS 238P - Operating Systems
 * logfs.c
 */

#include <pthread.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <linux/fs.h>
#include <unistd.h>
#include <fcntl.h>
#include "device.h"
#include "logfs.h"
#include "utils.h"

#define WCACHE_BLOCKS 32
#define RCACHE_BLOCKS 256

/**
 * Needs:
 *   pthread_create()
 *   pthread_join()
 *   pthread_mutex_init()
 *   pthread_mutex_destroy()
 *   pthread_mutex_lock()
 *   pthread_mutex_unlock()
 *   pthread_cond_init()
 *   pthread_cond_destroy()
 *   pthread_cond_wait()
 *   pthread_cond_signal()
 */

/* research the above Needed API and design accordingly */

/****************
 * WRITE BUFFER *
 ***************/

typedef struct write_buffer_t {
    // actual buffer used to accumulate writes
    void* buffer;

    // head, tail and size are in reference to above buffer
    size_t head;
    size_t tail;
    size_t size;

    size_t last_avail;
} write_buffer_t;

write_buffer_t *write_buffer_open(size_t buff_size) {
    write_buffer_t *write_buffer = calloc(sizeof(write_buffer_t), 1);
    if (!write_buffer) {
        perror("out of memory");
        return NULL;
    }

    write_buffer->buffer = calloc(buff_size, 1);
    if (!write_buffer->buffer) {
        perror("out of memory");
        free(write_buffer);
        return NULL;
    }

    write_buffer->head = 0;
    write_buffer->tail = buff_size - 1;
    write_buffer->size = buff_size;

    return write_buffer;
}

void write_buffer_close(write_buffer_t *write_buffer) {
    debugf("write_buffer_close\n");
    if (!write_buffer) return;

    free(write_buffer->buffer);
    free(write_buffer);
}

int write_buffer_add(write_buffer_t *write_buffer, const void *buf, size_t len) {

    size_t available = 0, head = write_buffer->head, tail = write_buffer->tail, size = write_buffer->size;

    if (head <= tail) {
        // head is before tail, so copy into write_buffer from head onwards

        available = tail - head + 1;

        if(len <= available) {
            memcpy(write_buffer->buffer + head, buf, len);
            head += len;
            head = head % size;
            debugf("write_buffer_add: 0 head [%d]->[%d]\n", write_buffer->head, head);
            write_buffer->head = head;
        } else {
            // no space
            return 1;
        }

    } else {
        // head is after tail, so copy into write_buffer from head onwards and wrap around

        available = (size - head) + tail + 1;

        if (len <= available){
            size_t space_left = size - head;

            // wrap around
            if (len > space_left) {
                size_t remaining = len - space_left;

                // Copy from head to end
                memcpy((char *)write_buffer->buffer + head, buf, space_left);
                /* Copy remaining from the start */
                memcpy(write_buffer->buffer, (char *)buf + space_left, remaining);
                /* Update head */
                remaining = (remaining) % size;
                debugf("write_buffer_add: 1 head [%d]->[%d]\n", write_buffer->head, remaining);
                write_buffer->head = remaining;

            } else {
                memcpy((char *)write_buffer->buffer + head, buf, len);
                /* Update head */
                head += len;
                head = head % size;
                debugf("write_buffer_add: 2 head [%d]->[%d]\n", write_buffer->head, head);
                write_buffer->head = head;
            }
        } else {
            // no space
            return 1;
        }
    }

    return 0;
}

/* get what needs to be written to disk*/
void* write_buffer_dequeue(write_buffer_t *write_buffer, size_t* disk_write_buff_size, int device_blk_sz, int dont_dequeue) {

    size_t available = 0, round_up_size = 0, head = write_buffer->head, tail = write_buffer->tail, size = write_buffer->size;

    void *disk_write_buf = NULL;

    int try = 0;
    retry:

    if (tail <= head) {

        available = head - tail - 1;

        debugf("write_buffer_dequeue: t[%d]<=h[%d] and have [%d] to write\n", tail, head, available);

        if (!available) {
            // 0 available
            debugf("write_buffer_dequeue: t[%d]<=h[%d] found 0 available\n", tail, head);
            return NULL;
        }

        // if avail is 4100, we want to write only 4096 of it. that means, we want to reduce head by 4 (4100-4096)
        if (!dont_dequeue && available > device_blk_sz) {
            size_t extra = available - device_blk_sz;
            // we need to move head back by extra
            head -= extra;
            head = head % size;

            debugf("write_buffer_dequeue: t[%d]<=h retrying with new head[%d]\n", tail, head);
            goto retry;
        }

        if (dont_dequeue) {
            if (available <= write_buffer->last_avail) {
                return NULL;
            }
            write_buffer->last_avail = available;
        } else {
            write_buffer->last_avail = 0;
        }

        round_up_size = roundUp(available, device_blk_sz);
        debugf("write_buffer_dequeue: av[%d] rus[%d] head[%d] tail[%d]\n", available, round_up_size, head, tail);

        if (available > round_up_size) {
            debugf("XXXXXXX\n");
            exit(0);
        }

        if (device_blk_sz <= 0 && available != round_up_size) {
            debugf("XXXXXXX nonono\n");
            exit(0);
        }

        // both these are return values
        disk_write_buf = aligned_alloc(512, round_up_size);
        *disk_write_buff_size = round_up_size;
        memset(disk_write_buf, 0, round_up_size);

        size_t t = tail + 1;
        t = t % size;

        memcpy(disk_write_buf, (char*)write_buffer->buffer + t, available);

        if (!dont_dequeue) {
            /* Update tail */
            tail += available;
            tail = tail % size;
            debugf("write_buffer_dequeue: 0 tail [%d]->[%d]\n", write_buffer->tail, tail);
            write_buffer->tail = tail;
        } else {
            debugf("write_buffer_dequeue: tail remains at [%d]\n", write_buffer->tail);
        }

    } else {

        available = (size - tail -1) + head;
        
        debugf("write_buffer_dequeue: t[%d]>h[%d] have [%d] to write\n", tail, head, available);

        if (!available) {
            debugf("write_buffer_dequeue: t[%d]>h[%d] found 0 available\n", tail, head);
            return NULL;
        }

        // if avail is 4100, we want to write only 4096 of it. that means, we want to reduce head by 4 (4100-4096)
        if (!dont_dequeue && available > device_blk_sz) {
            size_t extra = available - device_blk_sz;
            // we need to move head back by extra
            head -= extra;
            // head could have wrapped around to front
            head = head % size;

            debugf("write_buffer_dequeue: t[%d]>h retrying with new head[%d]\n", tail, head);
            goto retry;
        }

        if (dont_dequeue) {
            if (available <= write_buffer->last_avail) {
                return NULL;
            }
            write_buffer->last_avail = available;
        } else {
            write_buffer->last_avail = 0;
        }

        round_up_size = roundUp(available, device_blk_sz <= 0 ? available : device_blk_sz);

        if (device_blk_sz <= 0 && available != round_up_size) {
            debugf("XXXXXXX nonono 2\n");
            exit(0);
        }

        disk_write_buf = aligned_alloc(512, round_up_size);
        // disk_write_buf = calloc(round_up_size, 1);
        *disk_write_buff_size = round_up_size;
        memset(disk_write_buf, 0, round_up_size);

        size_t space_left = size - tail - 1;
        size_t remaining = available - space_left;

        size_t t = tail + 1;
        t = t % size;

        memcpy(disk_write_buf, write_buffer->buffer+t, space_left);
        memcpy(disk_write_buf+space_left, write_buffer->buffer, remaining);

        if (!dont_dequeue) {
            tail = remaining - 1;
            tail = tail % size;
            debugf("write_buffer_dequeue: 1 tail [%d]->[%d]\n", write_buffer->tail, tail);
            write_buffer->tail = tail;
        } else {
            debugf("write_buffer_dequeue: tail remains at [%d]\n", write_buffer->tail);
        }
    }

    return disk_write_buf;
}

/**************
 * READ CACHE *
 *************/

typedef struct cache_elem_t {
    uint64_t begin_offset;
    void* data;
    int valid;
} cache_elem_t;

typedef struct read_cache_t {
    int num_elems;
    size_t elem_size;

    // array of cache elements
    cache_elem_t* cache;
} read_cache_t;

void read_cache_close(read_cache_t *rc) {
    debugf("read_cache_close\n");
    if (!rc) return;

    if (rc->cache) {
        // Free the cache elements
        for (int i = 0; i < rc->num_elems; i++) {
            if (!rc->cache[i].data) continue;

            free(rc->cache[i].data);
        }

        // Free the cache array
        free(rc->cache);
    }

    // Free the read cache structure
    free(rc);
}

/* call with num_elems = #define RCACHE_BLOCKS, elem_size = blksize*/
read_cache_t *read_cache_open(int num_elems, size_t elem_size) {

    // Allocate memory for the read cache struct
    read_cache_t *rc = calloc(sizeof(read_cache_t), 1);
    if (!rc) {
        // Handle memory allocation failure
        perror("read cache open malloc failed");
        return NULL;
    }

    // Set the element size and number of elements in the read cache
    rc->elem_size = elem_size;
    rc->num_elems = num_elems;

    // Allocate memory for the cache array
    rc->cache = calloc(sizeof(cache_elem_t) * num_elems, 1);
    if (!rc->cache) {
        // Handle memory allocation failure
        read_cache_close(rc);
        perror("read_cache->cache malloc failed");
        return NULL;
    }

    // Initialize each element in the cache
    for (int i = 0; i < num_elems; i++) {
        cache_elem_t *ce = &rc->cache[i];
        ce->begin_offset = 0;
        ce->valid = 0;

        // Allocate memory for the data in each element
        ce->data = calloc(elem_size, 1);
        if (!ce->data) {
            // Handle memory allocation failure
            read_cache_close(rc);
            perror("read cache element malloc failed");
            return NULL;
        }
    }

    return rc;
}

void read_cache_add(read_cache_t *rc, void *buff, uint64_t offset) {

    int found_index = -1;

    // Iterate through rc.cache to find valid = 0
    for (int i = 0; i < rc->num_elems; i++) {
        if (!rc->cache[i].valid) {
            // found an unused cache element
            debugf("read_cache_add[%d]: found unused[%d]\n", offset, i);
            found_index = i;
            break;
        }
    }

    // If no invalid entry found, choose the first element in rc.cache array
    if (found_index == -1) {
        found_index = rand() % rc->num_elems;
        debugf("read_cache_add[%d]: replacing[%d]\n", offset, found_index);
    }

    cache_elem_t found_elem = rc->cache[found_index];

    // Set the found entry as valid
    found_elem.valid = 1;

    // Copy data from buff to the cache
    memcpy(found_elem.data, buff, rc->elem_size);

    // Set the begin_offset for the cache entry
    found_elem.begin_offset = offset;

    rc->cache[found_index] = found_elem;
}

int read_cache_lookup(read_cache_t *rc, cache_elem_t *ret_elem, uint64_t offset, size_t len) {

    // Round down offset to the nearest multiple of block size
    uint64_t rounded_down_offset = offset / rc->elem_size * rc->elem_size;

    // Iterate through the cache to check if rounded down offset matches a begin offset
    for (int i = 0; i < rc->num_elems; i++) {
        cache_elem_t elem = rc->cache[i];
        if (rounded_down_offset == elem.begin_offset) {
            if (elem.valid) {
                debugf("read_cache_lookup[%d]: cache hit at [%d]\n", offset, i);
                *ret_elem = elem;
                return 1;
            } else {
                // debugf("read_cache_lookup[%d]: found but invalidated [%d]\n", offset, i);
            }
        }
    }

    debugf("read_cache_lookup[%d]: not found\n", offset);
    // Return 1 if not found in the cache
    return 0;
}

void read_cache_invalidate(read_cache_t *rc, uint64_t offset, size_t len) {
    debugf("read_cache_invalidate[%d]: start\n", offset);

    // Round down offset to the nearest multiple of block size
    uint64_t rounded_down_offset = offset / rc->elem_size * rc->elem_size;

    if (rounded_down_offset != offset) {
        // debugf("Nonono\n");
        exit(0);
    }

    // Iterate through the cache to check if rounded down offset matches a begin offset
    for (int i = 0; i < rc->num_elems; i++) {
        cache_elem_t elem = rc->cache[i];
        if (rounded_down_offset == elem.begin_offset && elem.valid) {
            debugf("read_cache_invalidate[%d]: found[%d]\n", offset, i);
            elem.valid = 0;
            memset(elem.data, 0, rc->elem_size);
            rc->cache[i] = elem;
        }
    }
    
    debugf("read_cache_invalidate[%d]: done\n", offset);
}

/**********
 * LOG FS *
 **********/

typedef struct logfs {
    struct device *device;

    // pwrite to this offset to append
    uint64_t tail_offset;

    // buffer into which writes are accumulated before device write
    write_buffer_t *write_buffer;

    // direct mapped read cache
    read_cache_t *read_cache;

    // background thread to call device write
    pthread_t flusher_pthread;

    // to indicate that flusher needs to flush
    pthread_mutex_t do_flush_m;
    pthread_cond_t do_flush_cv;
    int flusher_started;
    int flush_available;
    int dont_dequeue_wb;
    int exit_flusher;

    // to indicate that flusher is done
    pthread_mutex_t done_flush_m;
    pthread_cond_t done_flush_cv;
    int flush_in_progress;
} logfs;

int flusher(logfs *lfs) {
    debugf("flusher: started\n");

    lfs->flusher_started = 1;

    int i = 0;
    for(;;i++) {
        debugf("flusher[%d]: start\n", i);

        // keep waiting while:
        // 1) we DON'T want to exit flusher AND
        // 2) write buffer DOESNT have something to flush
        while(!lfs->exit_flusher && !lfs->flush_available) {
            debugf("flusher[%d]: waiting\n", i);
            int rv = pthread_cond_wait(&lfs->do_flush_cv, &lfs->do_flush_m);
            debugf("flusher[%d]: done waiting rv[%d]\n", i, rv);
        }

        if (lfs->flush_available) {
            debugf("flusher[%d]: got flush available with dont_dequeue[%d]\n", i, lfs->dont_dequeue_wb);

            // perform the flush

            // get data to write to device from write buffer
            size_t disk_buf_size = 0;
            void *disk_write_buf = write_buffer_dequeue(lfs->write_buffer, &disk_buf_size, device_block(lfs->device), lfs->dont_dequeue_wb);

            if (!disk_buf_size) {
                // got 0 size from write buffer - possibly empty write buffer
                debugf("flusher[%d]: got 0 size from write buffer, skipping\n", i);

            } else if (!disk_write_buf) {
                // got NULL buffer from write buffer - possibly empty write buffer
                debugf("flusher[%d]: got NULL from write buffer, skipping\n", i);

            } else {
                // got valid buffer from write buffer - write to device
                debugf("flusher[%d]: got valid buffer from write buffer, writing at tail_offset[%d] buf[%p][%d]\n", i, lfs->tail_offset, disk_write_buf, disk_buf_size);

                debugf("\tFLUSH -- 0\n");
                print_buf(disk_write_buf, disk_buf_size);
                debugf("\tFLUSH -- 1\n");

                if ((device_write(lfs->device, disk_write_buf, lfs->tail_offset, disk_buf_size) != 0)) {
                    // device write failed

                    debugf("flusher[%d]: device write failed\n", i);
                    perror("flusher: device_write failed");

                } else {
                    // device write success

                    read_cache_invalidate(lfs->read_cache, lfs->tail_offset, disk_buf_size);

                    if (!lfs->dont_dequeue_wb) {
                        // do dequeue: from write
                        lfs->tail_offset += disk_buf_size;
                        debugf("flusher[%d]: device write success, updating tail_offset to [%d]\n", i, lfs->tail_offset);
                    } else {
                        // don't dequeue: from read
                        lfs->dont_dequeue_wb = 0;
                        debugf("flusher[%d]: device write success, without dequeue, tail_offset remains at [%d]\n", i, lfs->tail_offset);
                    }
                }

                // done writing, destroy the temporary buffer
                free(disk_write_buf);
                disk_buf_size = 0;
            }

            // mark that flush is done, so that next iter, we go back to waiting
            lfs->flush_available = 0;
        }

        if (lfs->exit_flusher) {
            debugf("flusher[%d]: got exit flusher\n", i);
            return 0;
        }

        debugf("flusher[%d]: done, signalling done_flush ..\n", i);
        
        pthread_mutex_lock(&lfs->done_flush_m);
        lfs->flush_in_progress = 0;
        pthread_cond_signal(&lfs->done_flush_cv);
        pthread_mutex_unlock(&lfs->done_flush_m);
        
        debugf("flusher[%d]: signalled done_flush!\n", i);
    }

    debugf("flusher: done\n");
    return 0;
}

/**
 * Opens the block device specified in pathname for buffered I/O using an
 * append only log structure.
 *
 * pathname: the pathname of the block device
 *
 * return: an opaque handle or NULL on error
 */

logfs *logfs_open(const char *pathname) {
    debugf("logfs_open: start\n");

    logfs *lfs = calloc(sizeof(logfs), 1);
    if (!lfs) {
        perror("out of memory");
        return NULL;
    }

    /* Initialize the tail_offset and other fields; set tail_offset only for clarity */
    lfs->tail_offset = 0;

    // Open the device
    lfs->device = device_open(pathname);
    if (!lfs->device) {
        perror("device_open failed");
        logfs_close(lfs);
        return NULL;
    }

    // Initialize the write buffer
    lfs->write_buffer = write_buffer_open(WCACHE_BLOCKS * device_block(lfs->device));
    if (!lfs->write_buffer) {
        perror("write_buffer_open failed");
        logfs_close(lfs);
        return NULL;
    }

    // Initialize the read cache
    lfs->read_cache = read_cache_open(RCACHE_BLOCKS, device_block(lfs->device));
    if (!lfs->read_cache) {
        perror("read_cache_open failed");
        logfs_close(lfs);
        return NULL;
    }

    // init mutex / CVs
    pthread_mutex_init(&lfs->do_flush_m, NULL);
    pthread_cond_init(&lfs->do_flush_cv, NULL);

    pthread_mutex_init(&lfs->done_flush_m, NULL);
    pthread_cond_init(&lfs->done_flush_cv, NULL);

    // lock to wait on flusher_started
    pthread_mutex_lock(&lfs->do_flush_m);

    // Spawn the consumer thread
    if (pthread_create(&lfs->flusher_pthread, NULL, (void *(*)(void *))flusher, lfs) != 0) {
        perror("pthread_create failed");
        logfs_close(lfs);
        return NULL;
    }

    // wait for flusher_started to become 1
    int i = 0;
    for(;;i++) {
        debugf("waiting for flusher to start[%d]\n", i);

        if(!pthread_mutex_trylock(&lfs->do_flush_m)) {
            debugf("waiting for flusher to start[%d]: locked successfully\n", i);
            // locked successfully
            int flusher_started = lfs->flusher_started;

            // unlock once read from lfs
            pthread_mutex_unlock(&lfs->do_flush_m);

            if (flusher_started) {
                debugf("waiting for flusher to start[%d]: locked successfully, and flusher started\n", i);
                break;
            }
            
            debugf("waiting for flusher to start[%d]: locked successfully, and flusher NOT started\n", i);
        }

        // could not lock, wait and retry
    }

    debugf("logfs_open: done\n");
    return lfs;
}

void logfs_set_tail_offset(logfs *lfs, uint64_t offset) {
    debugf("logfs_set_tail_offset: add [%d] to [%d]\n", offset, lfs->tail_offset);
    lfs->tail_offset += offset;
}

size_t logfs_wb_avail(logfs *lfs) {
    size_t amount_buffered = 0;
    write_buffer_t *wb = lfs->write_buffer;
    if(wb->head > wb->tail) {
        amount_buffered = wb->head - wb->tail - 1;
    } else {
        amount_buffered = (wb->size - wb->tail - 1) + wb->head;
    }
    return amount_buffered;
}

void logfs_wb_add(logfs *lfs, void *buf, size_t buf_len) {
    write_buffer_t *wb = lfs->write_buffer;

    debugf("logfs_wb_add: start, adding [%d] bytes wb\n", buf_len);

    int rv = write_buffer_add(wb, buf, buf_len);
    wb->last_avail += buf_len;
    
    debugf("logfs_wb_add: done adding [%d] bytes wb rv [%d]\n", buf_len, rv);
}

void logfs_wb_deq(logfs *lfs) {
    write_buffer_t *wb = lfs->write_buffer;

    size_t sz = 0;
    void *buf = write_buffer_dequeue(wb, &sz, device_block(lfs->device), 0);
    if (sz == 0) {

    } else if (buf == NULL) {

    } else {
        free(buf);
    }
}

/**
 * Closes a previously opened logfs handle.
 *
 * logfs: an opaque handle previously obtained by calling logfs_open()
 *
 * Note: logfs may be NULL.
 */

void logfs_close(logfs *lfs){
    debugf("logfs_close: start\n");

    if (!lfs) return;

    // signal flusher to flush
    // signal flusher to stop
    pthread_mutex_lock(&lfs->do_flush_m);
    lfs->flush_available = 1;
    lfs->exit_flusher = 1;
    pthread_cond_signal(&lfs->do_flush_cv);
    pthread_mutex_unlock(&lfs->do_flush_m);

    // wait for the consumer to stop waiting and return
    pthread_join(lfs->flusher_pthread, NULL);

    debugf("logfs_close: joined\n");

    write_buffer_close(lfs->write_buffer);
    read_cache_close(lfs->read_cache);
    device_close(lfs->device);

    debugf("logfs_close: done tail_offset[%d]\n", lfs->tail_offset);

    free(lfs);
}

/**
 * Append len bytes to the logfs.
 *
 * logfs: an opaque handle previously obtained by calling logfs_open()
 * buf  : a region of memory holding the len bytes to be written
 * len  : the number of bytes to write
 *
 * return: 0 on success, otherwise error
 */

int logfs_append(logfs *lfs, const void *buf, uint64_t len){
    debugf("logfs_append: start len[%d]\n", len);

    debugf("\tAPPEND -- 0\n");
    print_buf((void*) buf, len);
    debugf("\tAPPEND -- 1\n");

    // lock to (1) access the write buffer AND (2) change flush_available if needed
    pthread_mutex_lock(&lfs->do_flush_m);
    debugf("logfs_append: locked\n");

    write_buffer_t *wb = lfs->write_buffer;

    // add incoming write to the write buffer.
    int rv = 0;
    while((rv = write_buffer_add(wb, buf, len)) != 0) {
        // FULL
        debugf("logfs_append: write buffer is full, signalling flusher!\n");

        pthread_mutex_lock(&lfs->done_flush_m);
        lfs->flush_in_progress = 1;
        pthread_mutex_unlock(&lfs->done_flush_m);

        // signal flusher to flush
        lfs->flush_available = -1;
        lfs->dont_dequeue_wb = 0;
        pthread_cond_signal(&lfs->do_flush_cv);
        pthread_mutex_unlock(&lfs->do_flush_m);

        pthread_mutex_lock(&lfs->done_flush_m);
        while(lfs->flush_in_progress) {
            pthread_cond_wait(&lfs->done_flush_cv, &lfs->done_flush_m);
        }
        pthread_mutex_unlock(&lfs->done_flush_m);

        pthread_mutex_lock(&lfs->do_flush_m);
    }

    // incoming write has been added to the write buffer

    // check if enough data is accumulated in the write buffer
    uint64_t amount_buffered = 0;
    if(wb->head > wb->tail) {
        amount_buffered = wb->head - wb->tail - 1;
    } else {
        amount_buffered = (wb->size - wb->tail - 1) + wb->head;
    }

    if(amount_buffered >= device_block(lfs->device)){
        debugf("logfs_append: accumulated enough, need to flush! amount_buffd[%d] blk[%d]\n", amount_buffered, device_block(lfs->device));

        pthread_mutex_lock(&lfs->done_flush_m);
        lfs->flush_in_progress = 1;
        pthread_mutex_unlock(&lfs->done_flush_m);

        // signal flusher to flush
        lfs->flush_available = -1;
        lfs->dont_dequeue_wb = 0;
        pthread_cond_signal(&lfs->do_flush_cv);
    }

    // possible signal in previous if-block would get triggered here
    pthread_mutex_unlock(&lfs->do_flush_m);
    // if this append triggered a flush, next append's lock will only go through once flush is done

    pthread_mutex_lock(&lfs->done_flush_m);
    while(lfs->flush_in_progress) {
        pthread_cond_wait(&lfs->done_flush_cv, &lfs->done_flush_m);
    }
    pthread_mutex_unlock(&lfs->done_flush_m);

    debugf("logfs_append: done\n");
    return 0;
}


/**
 * Random read of len bytes at location specified in off from the logfs.
 *
 * logfs: an opaque handle previously obtained by calling logfs_open()
 * buf  : a region of memory large enough to receive len bytes
 * off  : the starting byte offset
 * len  : the number of bytes to read
 *
 * return: 0 on success, otherwise error
 */

int logfs_read(logfs *lfs, void *buf, uint64_t off, size_t len) {
    return logfs_read_with_flush(lfs, buf, off, len, 1);
}

int logfs_read_with_flush(logfs *lfs, void *buf, uint64_t off, size_t len, int flush_before_read) {
    debugf("logfs_read: start off[%d] len[%d] ... signalling\n", off, len);

    if (flush_before_read) {
        pthread_mutex_lock(&lfs->done_flush_m);
        lfs->flush_in_progress = 1;
        pthread_mutex_unlock(&lfs->done_flush_m);
        // First, need to ensure that the write buffer is gone to disk without emptying it
        pthread_mutex_lock(&lfs->do_flush_m);
        lfs->flush_available = 1;
        lfs->dont_dequeue_wb = 1;
        pthread_cond_signal(&lfs->do_flush_cv);
        pthread_mutex_unlock(&lfs->do_flush_m);

        pthread_mutex_lock(&lfs->done_flush_m);
        // wait on done_flush
        while (lfs->flush_in_progress) {
            pthread_cond_wait(&lfs->done_flush_cv, &lfs->done_flush_m);
        }
        pthread_mutex_unlock(&lfs->done_flush_m);
        
        debugf("logfs_read: flush done\n");
    }

    // At this point, write buffer is empty and all writes have gone to device, so it is safe to read from device now

    // Call read_cache_lookup
    read_cache_t *rc = lfs->read_cache;
    cache_elem_t cached_elem;

    pthread_mutex_lock(&lfs->do_flush_m);

    while (len > 0) {

        int is_cached = read_cache_lookup(rc, &cached_elem, off, len);

        if (is_cached) {
            // If found in the cache, copy data from the cache
            // Copy data from cached_elem to buf
            int index = off - cached_elem.begin_offset;
            void *data = (char *)cached_elem.data + index;

            size_t available = rc->elem_size - index;

            size_t to_cpy = len <= available ? len : available;

            debugf("logfs_read[%d]: is_cached to_cpy[%d] bytes from off[%d] available[%d] index[%d]\n", len, to_cpy, off, available, index);

            memcpy(buf, data, to_cpy);

            len -= to_cpy;
            off = off + to_cpy;
            buf = (char*) buf + to_cpy;

            debugf("logfs_read: found in cache, done len[%d]\n", len);            

        } else {
            // If not found in the cache

            uint64_t rounded_down_off = (off / rc->elem_size) * rc->elem_size;
            void *tmp_buff = aligned_alloc(512, rc->elem_size);
            size_t tb_sz = rc->elem_size;// device_block(lfs->device);

            // Read from the device to tmp_buff
            if (device_read(lfs->device, tmp_buff, rounded_down_off, tb_sz) != 0) {
                perror("device_read failed");
                free(tmp_buff);

                debugf("logfs_read: error done\n");
                return -1;
            }

            debugf("\tREAD -- 0\n");
            print_buf(tmp_buff, tb_sz);
            debugf("\tREAD -- 1\n");

            // Add to cache read_cache_add
            read_cache_add(rc, tmp_buff, rounded_down_off);

            // Copy data from tmp_buff to buf
            int index = off - rounded_down_off;
            void *data = (char *)tmp_buff + index;

            size_t available = tb_sz - index;

            size_t to_cpy = len <= available ? len : available;

            debugf("logfs_read[%d]: to_cpy[%d] bytes from off[%d] available[%d] index[%d]\n", len, to_cpy, off, available, index);

            memcpy(buf, data, to_cpy);

            len -= to_cpy;
            off = off + to_cpy;
            buf = (char*) buf + to_cpy;

            // Free the temporary buffer
            free(tmp_buff);

            debugf("logfs_read: added to cache, done len[%d]\n", len);
        }
    }
    
    pthread_mutex_unlock(&lfs->do_flush_m);
    return 0;
}