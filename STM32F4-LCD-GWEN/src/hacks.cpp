/*
 * LibraryHacks.cpp
 *
 *  Created on: 23 Jan 2011
 *      Author: Andy
 */

#include <string>
#include <algorithm>


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
