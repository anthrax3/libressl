include $(top_srcdir)/Makefile.am.common

bin_PROGRAMS = client_server

client_server_SOURCES = client_server.c

client_server_LDADD = $(top_builddir)/crypto/libcrypto.la
client_server_LDADD += $(top_builddir)/ssl/libssl.la
#client_server_LDFLAGS = $(PLATFORM_LDFLAGS)
