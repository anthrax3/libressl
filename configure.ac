AC_INIT([libopenssl], [1.0.1])
AM_INIT_AUTOMAKE([subdir-objects])
AC_CONFIG_MACRO_DIR([m4])
AC_PROG_CC

AC_PROG_LIBTOOL
AC_PROG_CC_STDC
CFLAGS="$CFLAGS -Wall -std=c99"

m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

AC_CANONICAL_HOST
case $host_os in
	*linux*)
		AC_DEFINE(NOT_BSD, 1)
		CFLAGS="$CFLAGS -D_BSD_SOURCE -D_POSIX_SOURCE -D_GNU_SOURCE" ;;
	*solaris*)
		AC_DEFINE(NOT_BSD, 1)
		CFLAGS="$CFLAGS -D__EXTENSIONS__" ;;
	*darwin*)
		AC_SUBST([PLATFORM_LDADD], ['-lpcap -levent_pthreads']) ;;
	*) ;;
esac

AC_CHECK_FUNC(strlcpy,[AC_SEARCH_LIBS(strlcpy,, [NO_STRLCPY=],
			  [NO_STRLCPY=yes])], [NO_STRLCPY=yes])
AC_SUBST(NO_STRLCPY)
AM_CONDITIONAL(NO_STRLCPY, test "x$NO_STRLCPY" = "xyes")

AC_CHECK_FUNC(strlcat,[AC_SEARCH_LIBS(strlcat,, [NO_STRLCAT=],
			  [NO_STRLCAT=yes])], [NO_STRLCAT=yes])
AC_SUBST(NO_STRLCAT)
AM_CONDITIONAL(NO_STRLCAT, test "x$NO_STRLCAT" = "xyes")

AC_CHECK_FUNC(reallocarray,[AC_SEARCH_LIBS(reallocarray,, [NO_REALLOCARRAY=],
			  [NO_REALLOCARRAY=yes])], [NO_REALLOCARRAY=yes])
AC_SUBST(NO_REALLOCARRAY)
AM_CONDITIONAL(NO_REALLOCARRAY, test "x$NO_REALLOCARRAY" = "xyes")

LT_INIT

AC_OUTPUT(Makefile include/Makefile include/openssl/Makefile ssl/Makefile crypto/Makefile tests/Makefile)
