#include "gui.h"

testControl::testControl(const Gwen::Controls::Base& base) : Base(base) {

	Controls::Button* pButton1 = new Controls::Button( this );
	pButton1->SetBounds( 10, 10, 100, 50 );
	pButton1->SetText( "Button1" );
	pButton1->onPress.Add( this, &testEventHandler::Button1_pressed );

	//Controls::Button* pButton2 = new Controls::Button( &pCanvas );
	//pButton2->SetBounds( 120, 10, 100, 50 );
	//pButton2->SetText( "Button2" );
	//pButton2->onPress.Add( &pCanvas, &testEventHandler::Button2_pressed );

	//Controls::ProgressBar* pb = new Controls::ProgressBar( &pCanvas );
	//pb->SetBounds(10, 70, 210, 30);
	//pb->SetValue( 0.27f );

}
