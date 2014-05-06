include $(top_srcdir)/Makefile.am.common

bin_PROGRAMS = http_client

http_client_SOURCES = http_client.c

http_client_LDADD = $(top_builddir)/crypto/libcrypto.la
http_client_LDADD += $(top_builddir)/ssl/libssl.la

TESTS =
check_PROGRAMS =
