/*
 * issetugid implementation for Linux/Solaris
 * See http://mcarpenter.org/blog/2013/01/15/solaris-issetugid(2)-bug for Solaris issues
 * Public domain
 */

#include <unistd.h>
#include <sys/types.h>

/*
 * Linux-specific glibc 2.16+ interface for determining if a process was launched setuid/setgid
 */
#ifdef HAVE_GETAUXVAL
#include <sys/auxv.h>
#endif

int issetugid(void)
{
#ifdef HAVE_GETAUXVAL
	if (getauxval(AT_SECURE) != 0) {
		return 1;
	}
#endif
	return (getuid() != geteuid()) || (getgid() != getegid());
}
