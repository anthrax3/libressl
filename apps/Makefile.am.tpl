include $(top_srcdir)/Makefile.am.common

bin_PROGRAMS = openssl

openssl_LDADD = $(top_builddir)/crypto/libcrypto.la
openssl_LDADD += $(top_builddir)/ssl/libssl.la

openssl_SOURCES =
noinst_HEADERS =
