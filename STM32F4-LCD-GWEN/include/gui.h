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
	GWEN_CONTROL(testControl, Controls::Base); // This is using Gwen's macro to declare the class constructor and a few needed functions.

	void Button1_pressed(Controls::Base* control) {
		//chprintf((BaseSequentialStream *)&SD2, "Button 1 Pressed\r\n");
		m_pgb1->SetValue(m_pgb1->GetValue()-0.1f);
		m_slider1->SetFloatValue(m_pgb1->GetValue()*100);
		Pgb1_changed(this);
	}
	void Button2_pressed(Controls::Base* control) {
		//chprintf((BaseSequentialStream *)&SD2, "Button 2 Pressed\r\n");
		m_pgb1->SetValue(m_pgb1->GetValue()+0.1f);
		m_slider1->SetFloatValue(m_pgb1->GetValue()*100);
		Pgb1_changed(this);
	}
	void Pgb1_changed(Controls::Base* control) {
		std::string tmp("PGB1 val changed: ");
		tmp += Utility::ToString(m_pgb1->GetValue()*100)+"\r\n";
		chprintf((BaseSequentialStream *)&SD2, tmp.c_str());
	}

	void Slider1_changed(Controls::Base* control) {
		m_pgb1->SetValue(m_slider1->GetFloatValue()/100);
		std::string tmp("Slider1 val changed: ");
		tmp += Utility::ToString(m_slider1->GetFloatValue())+"\r\n";
		chprintf((BaseSequentialStream *)&SD2, tmp.c_str());
	}

	void OnComboSelect( Gwen::Controls::Base* pControl )
	{
		std::string tmp("Combo Changed: ");
		tmp += combo1->GetSelectedItem()->GetText().Get()+"\r\n";
		chprintf((BaseSequentialStream *)&SD2, tmp.c_str());
	}

private:
	Controls::Button* m_but1;
	Controls::Button* m_but2;
	Controls::ProgressBar* m_pgb1;
	Controls::HorizontalSlider* m_slider1;
	Controls::ComboBox* combo1;
};


