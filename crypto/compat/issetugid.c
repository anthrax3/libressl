/*
 * Simple issetugid implementation for Linux/Solaris
 * Public domain
 */

#include <unistd.h>
#include <sys/types.h>

int issetugid(void)
{
   return (getuid() != geteuid()) || (getgid() != getegid());
}
