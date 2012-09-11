#include "gui.h"

GWEN_CONTROL_CONSTRUCTOR(testControl)
{

	m_but1 = new Controls::Button( this );
	m_but1->SetBounds( 10, 10, 100, 50 );
	m_but1->SetText( "Button1" );
	m_but1->onPress.Add( this, &testControl::Button1_pressed );

	m_but2 = new Controls::Button( this );
	m_but2->SetBounds( 120, 10, 100, 50 );
	m_but2->SetText( "Button2" );
	m_but2->onPress.Add( this, &testControl::Button2_pressed );

	m_pgb1 = new Controls::ProgressBar( this );
	m_pgb1->SetBounds(10, 70, 210, 30);
	m_pgb1->SetValue( 0.27f );

}
