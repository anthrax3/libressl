#ifndef _COMPAT_BYTE_ORDER_H_
#define _COMPAT_BYTE_ORDER_H_

#ifdef __linux__
#include <endian.h>
#else
#include_next <machine/endian.h>
#endif

#ifndef _BYTE_ORDER
#ifdef BYTE_ORDER
#define _BYTE_ORDER BYTE_ORDER
#endif
#endif

#ifndef _LITTLE_ENDIAN
#ifdef LITTLE_ENDIAN
#define _LITTLE_ENDIAN LITTLE_ENDIAN
#endif
#endif

#ifndef _BIG_ENDIAN
#ifdef BIG_ENDIAN
#define _BIG_ENDIAN BIG_ENDIAN
#endif
#endif

#endif
