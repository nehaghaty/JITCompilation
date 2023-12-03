/**
 * Tony Givargis
 * Copyright (C), 2023
 * University of California, Irvine
 *
 * CS 238P - Operating Systems
 * kvraw.c
 */

#include "logfs.h"
#include "kvraw.h"
#include "utils.h"

#define META_LEN ( sizeof (struct meta) )

#define KEY_OFF(o) ( (o) + META_LEN )
#define VAL_OFF(o) ( (o) + META_LEN + meta.key_len )

struct kvraw {
	uint64_t size;
	struct logfs *logfs;
};

#pragma pack(push, 1)
struct meta {
	char mark[2];
	uint64_t off;
	uint16_t key_len;
	uint32_t val_len;
};
#pragma pack(pop)

static int
read_meta(struct kvraw *kvraw, uint64_t off, struct meta *meta)
{
	memset(meta, 0, sizeof (struct meta));
	if ((off + META_LEN) > kvraw->size) {
		debugf("read_meta: corrupt kvraw->size[%d]\n", kvraw->size);
		TRACE("corrupt data");
		return -1;
	}
	if (logfs_read(kvraw->logfs, meta, off, META_LEN)) {
		TRACE(0);
		return -1;
	}

	debugf("read_meta: mark[0]=[%d] mark[1]=[%d] read at off[%d] keylen[%d] val_len[%d] kvraw->size[%d] meta.off[%d]\n",
		meta->mark[0], meta->mark[1], off, meta->key_len, meta->val_len, kvraw->size, meta->off);
	if (('K' != meta->mark[0]) ||
	    ('V' != meta->mark[1]) ||
	    ((off + META_LEN + meta->key_len + meta->val_len) > kvraw->size)) {

		sleep(1);
		TRACE("corrupt data");
		return -1;
	}
	return 0;
}

static int
read_meta2(struct kvraw *kvraw, uint64_t off, struct meta *meta)
{
	memset(meta, 0, sizeof (struct meta));
	if ((off + META_LEN) > kvraw->size) {
		debugf("read_meta2: corrupt kvraw->size[%d]\n", kvraw->size);
		TRACE("corrupt data");
		return -1;
	}
	if (logfs_read_with_flush(kvraw->logfs, meta, off, META_LEN, 0)) {
		TRACE(0);
		return -1;
	}

	debugf("read_meta2: mark[0]=[%d] mark[1]=[%d] off[%d], keylen[%d] val_len[%d] kvraw->size[%d]\n",
		meta->mark[0], meta->mark[1], off, meta->key_len, meta->val_len, kvraw->size);
	if (('K' != meta->mark[0]) ||
	    ('V' != meta->mark[1])) {

		// not corrupt, but read the end of the log, so stop the kvraw_visitor
		return -2;

	}
	return 0;
}

struct kvraw *
kvraw_open(const char *pathname, int restore)
{
	struct kvraw *kvraw = NULL;
	uint64_t off = 0;

	assert( safe_strlen(pathname) );

	if (!(kvraw = malloc(sizeof (struct kvraw)))) {
		TRACE("out of memory");
		return NULL;
	}
	memset(kvraw, 0, sizeof (struct kvraw));

	if (!(kvraw->logfs = logfs_open(pathname))) {
		kvraw_close(kvraw);
		TRACE(0);
		return NULL;
	}

	if (!restore) {
		// don't want to rewrite over when we want to restore
		if (kvraw_append(kvraw, "", 1, "", 1, &off)) { /* off = 0 */
			kvraw_close(kvraw);
			TRACE(0);
			return NULL;
		}
	}

	return kvraw;
}

void
kvraw_close(struct kvraw *kvraw)
{
	if (kvraw) {
		logfs_close(kvraw->logfs);
		memset(kvraw, 0, sizeof (struct kvraw));
	}
	FREE(kvraw);
}

struct kvraw_visitor_t kvraw_visitor_init(struct kvraw *kvr) {
	debugf("kvraw_visitor_init: start\n");

	struct kvraw_visitor_t v;
	v.offset = 0;
	v.key = 0;
	v.key_len = 0;
	v._valid = 0;

	// pretend like an append hap
	kvr->size += META_LEN;

	// read the first item
	struct meta meta;
	int rv = 0;
	if (read_meta2(kvr, v.offset, &meta)) {
		if (rv == -2) {
			// not an error, set invalid
			v._valid = 0;
			// wasnt a valid meta read, minus the meta len
			kvr->size -= META_LEN;
			debugf("kvraw_visitor_init: read_meta2 failed at offset[%d], setting invalid\n", v.offset);
			return v;
		}
		debugf("kvraw_visitor_init: read_meta2 failed\n");
		exit(0);
	}

	kvr->size += meta.key_len + meta.val_len;
	v.next_offset = META_LEN + meta.key_len + meta.val_len;
	v._valid = 1;

	// read the key
	void *key = calloc(meta.key_len, 1);
	logfs_read_with_flush(kvr->logfs, key, KEY_OFF(v.offset), meta.key_len, 0);

	void *val = calloc(meta.val_len, 1);
	logfs_read_with_flush(kvr->logfs, val, VAL_OFF(v.offset), meta.val_len, 0);

	size_t tmp_len = META_LEN + meta.key_len + meta.val_len;
	void *tmp = calloc(tmp_len, 1);
	memcpy(tmp, &meta, META_LEN);
	memcpy(tmp + META_LEN, key, meta.key_len);
	memcpy(tmp + META_LEN + meta.key_len, val, meta.val_len);
	logfs_wb_add(kvr->logfs, tmp, tmp_len);


	uint64_t amount_buffered = logfs_wb_avail(kvr->logfs);
	if (amount_buffered >= 4096) {
		logfs_wb_deq(kvr->logfs);
		logfs_set_tail_offset(kvr->logfs, 4096);
	}

	free(tmp);
	free(val);
	free(key);

	// go to the first item
	kvraw_visitor_next(kvr, &v);

	debugf("kvraw_visitor_init: done\n");
	return v;
}

int kvraw_visitor_valid(struct kvraw *kvr, struct kvraw_visitor_t *v) {
	return v->_valid;
}

// read and parse meta and key, free old key
void kvraw_visitor_next(struct kvraw *kvr, struct kvraw_visitor_t *v) {
	debugf("kvraw_visitor_next: start\n");

	kvr->size += META_LEN;

	// read meta
	struct meta meta;
	int rv = 0;

	if (rv = read_meta2(kvr, v->next_offset, &meta)) {
		if (rv == -2) {
			// not an error, set invalid
			v->_valid = 0;
			// wasnt a valid meta read, minus the meta len
			kvr->size -= META_LEN;
			debugf("kvraw_visitor_next: read_meta2 failed at offset[%d], setting invalid\n", v->offset);
			return;
		}
		debugf("kvraw_visitor_next: failed to read meta at offset[%d]\n", v->offset);
		exit(0);
	}
	v->offset = v->next_offset;
	kvr->size += meta.key_len + meta.val_len;
	v->next_offset = v->offset + META_LEN + meta.key_len + meta.val_len;
	v->key_len = meta.key_len;
	v->val_len = meta.val_len;

	if (v->key) {
		free(v->key);
		v->key = NULL;
	}

	// read the key
	v->key = calloc(v->key_len, 1);
	logfs_read_with_flush(kvr->logfs, v->key, KEY_OFF(v->offset), v->key_len, 0);

	void *val = calloc(meta.val_len, 1);
	logfs_read_with_flush(kvr->logfs, val, VAL_OFF(v->offset), meta.val_len, 0);

	size_t tmp_len = META_LEN + meta.key_len + meta.val_len;
	void *tmp = calloc(tmp_len, 1);
	memcpy(tmp, &meta, META_LEN);
	memcpy(tmp + META_LEN, v->key, meta.key_len);
	memcpy(tmp + META_LEN + meta.key_len, val, meta.val_len);
	logfs_wb_add(kvr->logfs, tmp, tmp_len);


	uint64_t amount_buffered = logfs_wb_avail(kvr->logfs);
	if (amount_buffered >= 4096) {
		logfs_wb_deq(kvr->logfs);
		logfs_set_tail_offset(kvr->logfs, 4096);
	}

	free(tmp);
	free(val);

	debugf("kvraw_visitor_next: done kvr->size[%d]\n", kvr->size);
}

void kvraw_visitor_close(struct kvraw *kvr, struct kvraw_visitor_t *v) {
	if (v->key) {
		free(v->key);
		v->key = NULL;
	}
}

int
kvraw_lookup(struct kvraw *kvraw,
	     void *key,
	     uint64_t *key_len, /* in/out */
	     void *val,
	     uint64_t *val_len, /* in/out */
	     uint64_t *off)     /* in/out */
{
	uint64_t key_len_, val_len_;
	struct meta meta;

	assert( kvraw );
	assert( key_len && (!(*key_len) || key) );
	assert( val_len && (!(*val_len) || val) );
	assert( off && (*off) );

	if (read_meta(kvraw, (*off), &meta)) {
		TRACE(0);
		return -1;
	}
	key_len_ = MIN(meta.key_len, (*key_len));
	val_len_ = MIN(meta.val_len, (*val_len));
	if (logfs_read(kvraw->logfs, key, KEY_OFF(*off), key_len_) ||
	    logfs_read(kvraw->logfs, val, VAL_OFF(*off), val_len_)) {
		TRACE(0);
		return -1;
	}
	(*key_len) = meta.key_len;
	(*val_len) = meta.val_len;
	(*off) = meta.off;
	return 0;
}

int
kvraw_append(struct kvraw *kvraw,
	     const void *key,
	     uint64_t key_len,
	     const void *val,
	     uint64_t val_len,
	     uint64_t *off)
{
	debugf("kvraw_append: start key_len[%d] val_len[%d]\n", key_len, val_len);
	struct meta meta;
	uint64_t off_;

	assert( kvraw );
	assert( key && key_len && (0xffff >= key_len) );
	assert( (!val_len || val) && (0xffffffff >= val_len) );
	assert( off );

	off_ = kvraw->size;
	meta.mark[0] = 'K';
	meta.mark[1] = 'V';
	meta.off = (*off);
	meta.key_len = (uint16_t)key_len;
	meta.val_len = (uint32_t)val_len;
	if (logfs_append(kvraw->logfs, &meta, META_LEN) ||
	    logfs_append(kvraw->logfs, key, meta.key_len) ||
	    logfs_append(kvraw->logfs, val, meta.val_len)) {
		TRACE(0);
		return -1;
	}
	kvraw->size += META_LEN + meta.key_len + meta.val_len;
	(*off) = off_;
	debugf("kvraw_append: done key_len[%d] val_len[%d] placed at offset[%d] parent[%d]\n", key_len, val_len, off_, meta.off);
	return 0;
}

// index:
// hash(key) -> latest_offset

// ins k1 v1
// h(k1) -> X1
// append() -> O1

// index: X1 -> O1

// ins k2 v2
// h(k2) -> X2
// append() -> O2

// index: X1 -> O1, X2 -> O2

// ins k3 v3
// h(k3) -> X1
// append() -> O3 (prev: O1)

// index: X1 -> O3, X2 -> O2