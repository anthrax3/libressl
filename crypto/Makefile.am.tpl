include $(top_srcdir)/Makefile.am.common

AM_CPPFLAGS += -I$(top_srcdir)/crypto/asn1
AM_CPPFLAGS += -I$(top_srcdir)/crypto/evp
AM_CPPFLAGS += -I$(top_srcdir)/crypto/modes

lib_LTLIBRARIES = libcrypto.la

libcrypto_la_LIBADD = libcompat.la libcompatnoopt.la
libcrypto_la_LIBADD += $(top_builddir)/libottery/libottery.la
libcrypto_la_LDFLAGS = -version-info 1:1:0
libcrypto_la_CFLAGS = $(CFLAGS) $(USER_CFLAGS) -DOPENSSL_NO_HW_PADLOCK

noinst_LTLIBRARIES = libcompat.la libcompatnoopt.la

# compatibility functions that need to be build without optimizations
libcompatnoopt_la_CFLAGS = -O0
libcompatnoopt_la_SOURCES = compat/explicit_bzero.c

# other compatibility functions
libcompat_la_CFLAGS = $(CFLAGS) $(USER_CFLAGS)
libcompat_la_SOURCES =
if NO_STRLCAT
libcompat_la_SOURCES += compat/strlcat.c
endif
if NO_STRLCPY
libcompat_la_SOURCES += compat/strlcpy.c
endif
if NO_REALLOCARRAY
libcompat_la_SOURCES += compat/reallocarray.c
endif
if NO_TIMINGSAFE_MEMCMP
libcompat_la_SOURCES += compat/timingsafe_memcmp.c
endif
if NO_TIMINGSAFE_BCMP
libcompat_la_SOURCES += compat/timingsafe_bcmp.c
endif
if NO_ARC4RANDOM_BUF
libcompat_la_SOURCES += compat/arc4random.c
endif
if NO_ISSETUGID
libcompat_la_SOURCES += compat/issetugid.c
endif

noinst_HEADERS = des/ncbc_enc.c
libcrypto_la_SOURCES =
EXTRA_libcrypto_la_SOURCES =
