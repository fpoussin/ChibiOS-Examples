// Newlib stubs implementation

#include <sys/types.h>
#include <stdlib.h>


pid_t _getpid()
{
  return 0;
}

void _exit( int status )
{
  while( 1 );
}

int _kill( int pid, int sig )
{
  return -1;
}

int _times_r ( struct _reent *r, struct tms *tmsbuf )
{
   return -1;
}

void *__dso_handle = NULL;





