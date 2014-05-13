AC_INIT([libressl], [24.0])
AM_INIT_AUTOMAKE([subdir-objects])
AC_CONFIG_MACRO_DIR([m4])

m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

AC_SUBST([USER_CFLAGS], "-O2 $CFLAGS")
CFLAGS="$CFLAGS -Wall -Werror -std=c99 -g"

AC_CANONICAL_HOST
case $host_os in
	*darwin*)
		LDFLAGS="$LDFLAGS -Qunused-arguments" ;;
	*linux*)
		CFLAGS="$CFLAGS -D_BSD_SOURCE -D_POSIX_SOURCE -D_GNU_SOURCE" ;;
	*solaris*)
		CFLAGS="$CFLAGS -D__EXTENSIONS__" ;;
	*) ;;
esac

AC_PROG_CC
AC_PROG_LIBTOOL
AC_PROG_CC_STDC
AM_PROG_CC_C_O

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

AC_CHECK_FUNC(bzero,[AC_SEARCH_LIBS(bzero,, [NO_BZERO=],
			  [NO_BZERO=yes])], [NO_BZERO=yes])
AC_SUBST(NO_BZERO)
AM_CONDITIONAL(NO_BZERO, test "x$NO_BZERO" = "xyes")

LT_INIT

ac_configure_args="$ac_configure_args CFLAGS=\"$CFLAGS -DNO_OPENSSL\" LDFLAGS=\"$LDFLAGS -L../crypto\""
AC_CONFIG_SUBDIRS([libottery])
AC_OUTPUT(Makefile include/Makefile include/openssl/Makefile ssl/Makefile
          crypto/Makefile tests/Makefile apps/Makefile)
