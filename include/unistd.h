/*
 * Simple issetugid implementation for Linux/Solaris
 * Public domain
 */

#include_next <unistd.h>

#ifndef LIBBSD_UNISTD_H
#define LIBBSD_UNISTD_H

__BEGIN_DECLS
int getentropy(void *buf, size_t buflen);
int issetugid(void);
__END_DECLS

#endif
