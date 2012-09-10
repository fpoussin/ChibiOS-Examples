extern "C" {
	#include "ch.h"
	#include "hal.h"
	#include "chprintf.h"
	#include "evtimer.h"
	#include "shell.h"
	#include "shellconfig.h"
}

#include "Gwen/Gwen.h"
#include "Gwen/Events.h"

#include "Gwen/Controls/Base.h"
#include "Gwen/Controls/Canvas.h"
#include "Gwen/Controls/Button.h"
#include "Gwen/Controls/ProgressBar.h"


using namespace Gwen;

class testEventHandler : public Event::Handler {

public:
	void Button1_pressed(Controls::Base* control) {

		chprintf((BaseSequentialStream *)&SD2, "\nButton 1 Pressed");
	}
	void Button2_pressed(Controls::Base* control) {

		chprintf((BaseSequentialStream *)&SD2, "\nButton 2 Pressed");
	}
};

class testControl : public Controls::Base {

public:
	testControl(const Gwen::Controls::Base& base);

};


