/**
 * Tony Givargis
 * Copyright (C), 2023
 * University of California, Irvine
 *
 * CS 238P - Operating Systems
 * kvraw.h
 */

#ifndef _KVRAW_H_
#define _KVRAW_H_

#include "system.h"

struct kvraw;

struct kvraw_visitor_t {
	uint64_t offset, next_offset;

	void *key;
	size_t key_len;
	size_t val_len;

	int _valid;
};

struct kvraw *kvraw_open(const char *pathname, int restore);

void kvraw_close(struct kvraw *kvraw);

int kvraw_lookup(struct kvraw *kvraw,
		 void *key,
		 uint64_t *key_len, /* in/out */
		 void *val,
		 uint64_t *val_len, /* in/out */
		 uint64_t *off);    /* in/out */

int kvraw_append(struct kvraw *kvraw,
		 const void *key,
		 uint64_t key_len,
		 const void *val,
		 uint64_t val_len,
		 uint64_t *off);


struct kvraw_visitor_t kvraw_visitor_init(struct kvraw *kvr);
int kvraw_visitor_valid(struct kvraw *kvr, struct kvraw_visitor_t *v);
void kvraw_visitor_next(struct kvraw *kvr, struct kvraw_visitor_t *v);
void kvraw_visitor_close(struct kvraw *kvr, struct kvraw_visitor_t *v);

#endif /* _KVRAW_H_ */
