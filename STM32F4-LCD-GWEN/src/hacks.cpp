/*
 * LibraryHacks.cpp
 *
 *  Created on: 23 Jan 2011
 *      Author: Andy
 */

#include <cstdlib>
#include <string>
#include <algorithm>
#include <sys/types.h>


/*
 * The default pulls in 70K of garbage
 */

namespace __gnu_cxx {

  void __verbose_terminate_handler() {
    for(;;);
  }
}


/*
 * The default pulls in about 12K of garbage
 */

extern "C" void __cxa_pure_virtual() {
  for(;;);
}


/*
 * Implement C++ new/delete operators using the heap
 */

void *operator new(size_t size) {
  return malloc(size);
}

void *operator new[](size_t size) {
  return malloc(size);
}

void operator delete(void *p) {
  free(p);
}

void operator delete[](void *p) {
  free(p);
}


/*
 * sbrk function for getting space for malloc and friends
 */
/* Using Chibios'
extern int  _end;

extern "C" {
  caddr_t _sbrk ( int incr ) {

    static unsigned char *heap = NULL;
    unsigned char *prev_heap;

    if (heap == NULL) {
      heap = (unsigned char *)&_end;
    }
    prev_heap = heap;
    /* check removed to show basic approach *//*

    heap += incr;

    return (caddr_t) prev_heap;
  }
}
*/
// Avoid using stringstream
std::string str_itoa(int value, short base = 10) {

	enum { kMaxDigits = 35 };

	std::string buf;
	buf.reserve( kMaxDigits ); // Pre-allocate enough space.

	// check that the base if valid
	if (base < 2 || base > 16) return buf;

	int quotient = value;
	// Translating number to string with base:
	do {
		buf += "0123456789abcdef"[ std::abs( quotient % base ) ];
		quotient /= base;

	} while ( quotient );

	// Append the negative sign for base 10
	if ( value < 0 && base == 10) buf += '-';

	std::reverse( buf.begin(), buf.end() );

	return buf;
}

std::string str_uitoa(unsigned int value, short base = 10) {

	enum { kMaxDigits = 35 };

	std::string buf;
	buf.reserve( kMaxDigits ); // Pre-allocate enough space.

	// check that the base if valid
	if (base < 2 || base > 16) return buf;

	int quotient = value;
	// Translating number to string with base:
	do {
		buf += "0123456789abcdef"[ std::abs( quotient % base ) ];
		quotient /= base;

	} while ( quotient );

	std::reverse( buf.begin(), buf.end() );

	return buf;
}
