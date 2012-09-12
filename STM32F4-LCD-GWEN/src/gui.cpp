#include "gui.h"

GWEN_CONTROL_CONSTRUCTOR(testControl)
{

	m_but1 = new Controls::Button( this );
	m_but1->SetBounds( 10, 10, 100, 50 );
	m_but1->SetText( "Decrease" );
	m_but1->onPress.Add( this, &testControl::Button1_pressed );

	m_but2 = new Controls::Button( this );
	m_but2->SetBounds( 120, 10, 100, 50 );
	m_but2->SetText( "Increase" );
	m_but2->onPress.Add( this, &testControl::Button2_pressed );

	m_pgb1 = new Controls::ProgressBar( this );
	m_pgb1->SetBounds(10, 70, 210, 30);
	m_pgb1->SetValue( 0.27f );

	pSlider1 = new Gwen::Controls::HorizontalSlider( this );
	pSlider1->SetPos( 10, 120 );
	pSlider1->SetSize( 150, 20 );
	pSlider1->SetRange( 0, 100 );
	pSlider1->SetFloatValue( 20 );
	pSlider1->SetNotchCount( 10 );
	pSlider1->SetClampToNotches( true );
	//pSlider1->onValueChanged.Add( this, &Slider::SliderMoved );

}
