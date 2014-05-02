/* OPENBSD ORIGINAL: lib/libc/crypto/arc4random.c */

/*	$OpenBSD: arc4random.c,v 1.25 2013/10/01 18:34:57 markus Exp $	*/

/*
 * Copyright (c) 1996, David Mazieres <dm@uun.org>
 * Copyright (c) 2008, Damien Miller <djm@openbsd.org>
 * Copyright (c) 2013, Markus Friedl <markus@openbsd.org>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

/*
 * ChaCha based random number generator for OpenBSD.
 */

#include <sys/types.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>

#ifndef HAVE_ARC4RANDOM

#define RANDOMDEV "/dev/urandom"

#include <openssl/rand.h>
#include <openssl/err.h>

#define KEYSTREAM_ONLY
#include "chacha_private.h"

#ifndef MIN
#define MIN(a, b) \
({ __typeof__(a) _a = (a); \
   __typeof__(b) _b = (b); \
   _a < _b ? _a : _b; })
#endif

#ifdef __GNUC__
#define inline __inline
#else				/* !__GNUC__ */
#define inline
#endif				/* !__GNUC__ */

#ifdef _WIN32
#include <windows.h>
static CRITICAL_SECTION arc4random_mtx;
#define THREAD_INIT() InitializeCriticalSection(&arc4random_mtx)
#define THREAD_LOCK() EnterCriticalSection(&arc4random_mtx)
#define THREAD_UNLOCK() LeaveCriticalSection(&arc4random_mtx)
#else
#include <pthread.h>
static pthread_mutex_t arc4random_mtx = PTHREAD_MUTEX_INITIALIZER;
#define THREAD_INIT()
#define THREAD_LOCK()   pthread_mutex_lock(&arc4random_mtx)
#define THREAD_UNLOCK() pthread_mutex_unlock(&arc4random_mtx)
#endif

#define KEYSZ	32
#define IVSZ	8
#define KEYSIZE (KEYSZ + IVSZ)
#define BLOCKSZ	64
#define RSBUFSZ	(16*BLOCKSZ)
static int rs_initialized;
static pid_t rs_stir_pid;
static chacha_ctx rs;		/* chacha context for random keystream */
static u_char rs_buf[RSBUFSZ];	/* keystream blocks */
static size_t rs_have;		/* valid bytes at end of rs_buf */
static size_t rs_count;		/* bytes till reseed */

static inline void _rs_rekey(u_char *dat, size_t datlen);

static inline void
_rs_init(u_char *buf, size_t n)
{
	if (n < KEYSZ + IVSZ)
		return;
	chacha_keysetup(&rs, buf, KEYSZ * 8, 0);
	chacha_ivsetup(&rs, buf + KEYSZ);
}

static void
_rs_stir(void)
{
	int done, fd;
	struct {
		struct timeval  tv;
		pid_t           pid;
		uint8_t         rnd[KEYSIZE];
	} rdat;

	fd = open(RANDOMDEV, O_RDONLY, 0);
	done = 0;
	if (fd >= 0) {
		if (read(fd, &rdat, KEYSIZE) == KEYSIZE)
			done = 1;
		(void)close(fd);
	}
	if (!done) {
		(void)gettimeofday(&rdat.tv, NULL);
		rdat.pid = getpid();
		/* We'll just take whatever was on the stack too... */
	}

	if (!rs_initialized) {
		rs_initialized = 1;
		THREAD_INIT();
		_rs_init((u_char *)&rdat, sizeof(rdat));
	} else
		_rs_rekey((u_char *)&rdat, sizeof(rdat));
	memset(&rdat, 0, sizeof(rdat));

	/* invalidate rs_buf */
	rs_have = 0;
	memset(rs_buf, 0, RSBUFSZ);

	rs_count = 1600000;
}

static inline void
_rs_stir_if_needed(size_t len)
{
	pid_t pid = getpid();

	if (rs_count <= len || !rs_initialized || rs_stir_pid != pid) {
		rs_stir_pid = pid;
		_rs_stir();
	} else
		rs_count -= len;
}

static inline void
_rs_rekey(u_char *dat, size_t datlen)
{
#ifndef KEYSTREAM_ONLY
	memset(rs_buf, 0,RSBUFSZ);
#endif
	/* fill rs_buf with the keystream */
	chacha_encrypt_bytes(&rs, rs_buf, rs_buf, RSBUFSZ);
	/* mix in optional user provided data */
	if (dat) {
		size_t i, m;

		m = MIN(datlen, KEYSZ + IVSZ);
		for (i = 0; i < m; i++)
			rs_buf[i] ^= dat[i];
	}
	/* immediately reinit for backtracking resistance */
	_rs_init(rs_buf, KEYSZ + IVSZ);
	memset(rs_buf, 0, KEYSZ + IVSZ);
	rs_have = RSBUFSZ - KEYSZ - IVSZ;
}

static inline void
_rs_random_buf(void *_buf, size_t n)
{
	u_char *buf = (u_char *)_buf;
	size_t m;

	_rs_stir_if_needed(n);
	while (n > 0) {
		if (rs_have > 0) {
			m = MIN(n, rs_have);
			memcpy(buf, rs_buf + RSBUFSZ - rs_have, m);
			memset(rs_buf + RSBUFSZ - rs_have, 0, m);
			buf += m;
			n -= m;
			rs_have -= m;
		}
		if (rs_have == 0)
			_rs_rekey(NULL, 0);
	}
}

static inline void
_rs_random_u32(u_int32_t *val)
{
	_rs_stir_if_needed(sizeof(*val));
	if (rs_have < sizeof(*val))
		_rs_rekey(NULL, 0);
	memcpy(val, rs_buf + RSBUFSZ - rs_have, sizeof(*val));
	memset(rs_buf + RSBUFSZ - rs_have, 0, sizeof(*val));
	rs_have -= sizeof(*val);
	return;
}

void
arc4random_stir(void)
{
	THREAD_LOCK();
	_rs_stir();
	THREAD_UNLOCK();
}

void
arc4random_addrandom(u_char *dat, int datlen)
{
	int m;

	THREAD_LOCK();
	if (!rs_initialized)
		_rs_stir();
	while (datlen > 0) {
		m = MIN(datlen, KEYSZ + IVSZ);
		_rs_rekey(dat, m);
		dat += m;
		datlen -= m;
	}
	THREAD_UNLOCK();
}

u_int32_t
arc4random(void)
{
	u_int32_t val;

	THREAD_LOCK();
	_rs_random_u32(&val);
	THREAD_UNLOCK();
	return val;
}

/*
 * If we are providing arc4random, then we can provide a more efficient
 * arc4random_buf().
 */
# ifndef HAVE_ARC4RANDOM_BUF
void
arc4random_buf(void *buf, size_t n)
{
	THREAD_LOCK();
	_rs_random_buf(buf, n);
	THREAD_UNLOCK();
}
# endif /* !HAVE_ARC4RANDOM_BUF */
#endif /* !HAVE_ARC4RANDOM */

/* arc4random_buf() that uses platform arc4random() */
#if !defined(HAVE_ARC4RANDOM_BUF) && defined(HAVE_ARC4RANDOM)
void
arc4random_buf(void *_buf, size_t n)
{
	size_t i;
	u_int32_t r = 0;
	char *buf = (char *)_buf;

	for (i = 0; i < n; i++) {
		if (i % 4 == 0)
			r = arc4random();
		buf[i] = r & 0xff;
		r >>= 8;
	}
	i = r = 0;
}
#endif /* !defined(HAVE_ARC4RANDOM_BUF) && defined(HAVE_ARC4RANDOM) */

#ifndef HAVE_ARC4RANDOM_UNIFORM
/*
 * Calculate a uniformly distributed random number less than upper_bound
 * avoiding "modulo bias".
 *
 * Uniformity is achieved by generating new random numbers until the one
 * returned is outside the range [0, 2**32 % upper_bound).  This
 * guarantees the selected random number will be inside
 * [2**32 % upper_bound, 2**32) which maps back to [0, upper_bound)
 * after reduction modulo upper_bound.
 */
u_int32_t
arc4random_uniform(u_int32_t upper_bound)
{
	u_int32_t r, min;

	if (upper_bound < 2)
		return 0;

	/* 2**32 % x == (2**32 - x) % x */
	min = -upper_bound % upper_bound;

	/*
	 * This could theoretically loop forever but each retry has
	 * p > 0.5 (worst case, usually far better) of selecting a
	 * number inside the range we need, so it should rarely need
	 * to re-roll.
	 */
	for (;;) {
		r = arc4random();
		if (r >= min)
			break;
	}

	return r % upper_bound;
}
#endif /* !HAVE_ARC4RANDOM_UNIFORM */

#if 0
/*-------- Test code for i386 --------*/
#include <stdio.h>
#include <machine/pctr.h>
int
main(int argc, char **argv)
{
	const int iter = 1000000;
	int     i;
	pctrval v;

	v = rdtsc();
	for (i = 0; i < iter; i++)
		arc4random();
	v = rdtsc() - v;
	v /= iter;

	printf("%qd cycles\n", v);
	exit(0);
}
#endif
