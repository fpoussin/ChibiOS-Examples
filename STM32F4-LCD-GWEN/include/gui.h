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
#include "Gwen/Controls.h"

using namespace Gwen;

class testControl : public Controls::Base {

public:
	GWEN_CONTROL(testControl, Controls::Base);

	void Button1_pressed(Controls::Base* control) {
		chprintf((BaseSequentialStream *)&SD2, "Button 1 Pressed\n");
		Pgb1_changed(this);
	}
	void Button2_pressed(Controls::Base* control) {
		chprintf((BaseSequentialStream *)&SD2, "Button 2 Pressed\n");
		Pgb1_changed(this);
	}
	void Pgb1_changed(Gwen::Controls::Base* control) {
		std::string tmp("PGB1 val changed: ");
		tmp += Gwen::Utility::ToString((unsigned int)m_pgb1->GetValue()*100);
		chprintf((BaseSequentialStream *)&SD2, tmp.c_str());
	}

private:
	Controls::Button* m_but1;
	Controls::Button* m_but2;
	Controls::ProgressBar* m_pgb1;
};


