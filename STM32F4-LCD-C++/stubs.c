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





