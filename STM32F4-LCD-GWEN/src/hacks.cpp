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
