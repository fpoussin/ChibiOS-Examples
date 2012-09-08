#include "Gwen/Gwen.h"

extern "C" {
	#include "ch.h"
	#include "hal.h"
	#include "chprintf.h"
	#include "evtimer.h"
	#include "shell.h"
}

namespace Gwen {

	class testHandler: public Event::Handler {

	public:

		void onPress( Controls::Base* control )
		{
			chprintf((BaseSequentialStream *)&SD2, "\nButton pushed");

		}
	};


	void onPress( Controls::Base* control )
	{
		chprintf((BaseSequentialStream *)&SD2, "\nButton pushed");

	}
}


