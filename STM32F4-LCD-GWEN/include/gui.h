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

	void Button1_pressed(Controls::Base* pControl)
	{
		(void)pControl;
		//chprintf((BaseSequentialStream *)&SD2, "Button 1 Pressed\r\n");
		m_pgb1->SetValue(m_pgb1->GetValue()-0.1f);
		m_slider1->SetFloatValue(m_pgb1->GetValue()*100);
		Pgb1_changed(this);
	}
	void Button2_pressed(Controls::Base* pControl)
	{
		(void)pControl;
		//chprintf((BaseSequentialStream *)&SD2, "Button 2 Pressed\r\n");
		m_pgb1->SetValue(m_pgb1->GetValue()+0.1f);
		m_slider1->SetFloatValue(m_pgb1->GetValue()*100);
		Pgb1_changed(this);
	}
	void Pgb1_changed(Controls::Base* pControl)
	{
		(void)pControl;
		std::string tmp("PGB1 val changed: ");
		tmp += Utility::ToString(m_pgb1->GetValue()*100)+"\r\n";
		chprintf((BaseSequentialStream *)&SD2, tmp.c_str());
	}

	void Slider1_changed(Controls::Base* pControl)
	{
		(void)pControl;
		m_pgb1->SetValue(m_slider1->GetFloatValue()/100);
		std::string tmp("Slider1 val changed: ");
		tmp += Utility::ToString(m_slider1->GetFloatValue())+"\r\n";
		chprintf((BaseSequentialStream *)&SD2, tmp.c_str());
	}

	void OnComboSelect( Controls::Base* pControl )
	{
		(void)pControl;
		std::string tmp("Combo Changed: ");
		tmp += m_combo1->GetSelectedItem()->GetText().Get()+"\r\n";
		chprintf((BaseSequentialStream *)&SD2, tmp.c_str());
	}

	void OpenWindow( Controls::Base* pControl )
	{
		(void)pControl;
		if (Controls::WindowControl::GetCount() < 10) {
			Controls::WindowControl* pWindow = new Controls::WindowControl( GetCanvas() );
			pWindow->SetTitle(std::string("Test Window ")+Utility::ToString(Controls::WindowControl::GetCount()));
			pWindow->SetSize( 50+(rand()%150), 50+(rand()%150) );
			pWindow->SetPos( 50+(rand()%150), 50+(rand()%50) );
			pWindow->SetDeleteOnClose( true );
		}
	}

private:
	Controls::Button* m_but1;
	Controls::Button* m_but2;
	Controls::Button* m_but3;
	Controls::ProgressBar* m_pgb1;
	Controls::HorizontalSlider* m_slider1;
	Controls::ComboBox* m_combo1;
};


