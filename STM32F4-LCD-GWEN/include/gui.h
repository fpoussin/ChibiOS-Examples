extern "C" {
	#include "ch.h"
	#include "hal.h"
	#include "chprintf.h"
}

#include "Gwen/Gwen.h"
#include "Gwen/Events.h"
#include "Gwen/Controls.h"

using namespace Gwen;

class testControl : public Controls::Base {

public:
	GWEN_CONTROL(testControl, Controls::Base);

	void Button1_pressed(Controls::Base* control) {
		//chprintf((BaseSequentialStream *)&SD2, "Button 1 Pressed\r\n");
		this->m_pgb1->SetValue(m_pgb1->GetValue()-0.1f);
		Pgb1_changed(this);
	}
	void Button2_pressed(Controls::Base* control) {
		//chprintf((BaseSequentialStream *)&SD2, "Button 2 Pressed\r\n");
		this->m_pgb1->SetValue(m_pgb1->GetValue()+0.1f);
		Pgb1_changed(this);
	}
	void Pgb1_changed(Gwen::Controls::Base* control) {
		std::string tmp("PGB1 val changed: ");
		tmp += Gwen::Utility::ToString((unsigned int)m_pgb1->GetValue()*100)+"\r\n";
		chprintf((BaseSequentialStream *)&SD2, tmp.c_str());
	}

private:
	Controls::Button* m_but1;
	Controls::Button* m_but2;
	Controls::ProgressBar* m_pgb1;
};


