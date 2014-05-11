/*
 * Copyright (c) 2014 Ted Unangst <tedu@openbsd.org>
 * Copyright (c) 2014 Brent Cook <busterb@gmail.com>
 *   modified to use libottery (which uses ChaCha like OpenBSD's arc4random)
 *   but has various additional seeding capabilities.
 *   See https://github.com/nmathewson/libottery
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
#include "cryptlib.h"
#include <openssl/rand.h>

#include <stdlib.h>
#include <ottery.h>

/*
 * The useful functions in this file are at the bottom.
 */
int
RAND_set_rand_method(const RAND_METHOD *meth)
{
	return 1;
}

const RAND_METHOD *
RAND_get_rand_method(void)
{
	return NULL;
}

RAND_METHOD *
RAND_SSLeay(void)
{
	return NULL;
}

#ifndef OPENSSL_NO_ENGINE
int
RAND_set_rand_engine(ENGINE *engine)
{
	return 1;
}
#endif

void
RAND_cleanup(void)
{

}

void
RAND_seed(const void *buf, int num)
{
	ottery_add_seed(buf, num);
}

void
RAND_add(const void *buf, int num, double entropy)
{
	ottery_add_seed(buf, num);
}

int
RAND_status(void)
{
	return 1;
}

int
RAND_poll(void)
{
       return 1;
}

/*
 * Hurray. You've made it to the good parts.
 */
int
RAND_bytes(unsigned char *buf, int num)
{
	if (num > 0)
		ottery_rand_bytes(buf, num);
	return 1;
}

int
RAND_pseudo_bytes(unsigned char *buf, int num)
{
	if (num > 0)
		ottery_rand_bytes(buf, num);
	return 1;
}
