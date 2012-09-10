#include "gui.h"

testEventHandler handler;

void runGui(void) {

	Renderer::ChibiGFX pRenderer = Renderer::ChibiGFX();
	Skin::Light skin;
	skin.SetRender( &pRenderer );

	const uint16_t width = pRenderer.getWidth();
	const uint16_t height = pRenderer.getHeight();
	Controls::Canvas pCanvas = Controls::Canvas( &skin );
	pCanvas.SetSize( width-1, height-1 );

	pCanvas.SetDrawBackground( true );
	pCanvas.SetBackgroundColor( Color( 0xBB, 0xBB, 0xBB, 0xFF ) );

	Controls::Button* pButton1 = new Controls::Button( &pCanvas );
	pButton1->SetBounds( 10, 10, 100, 50 );
	pButton1->SetText( "Button1" );
	pButton1->onPress.Add( &handler, &testEventHandler::Button1_pressed );

	Controls::Button* pButton2 = new Controls::Button( &pCanvas );
	pButton2->SetBounds( 120, 10, 100, 50 );
	pButton2->SetText( "Button2" );
	pButton1->onPress.Add( &handler, &testEventHandler::Button2_pressed );

	Controls::ProgressBar* pb = new Controls::ProgressBar( &pCanvas );
	pb->SetBounds(10, 70, 210, 30);
	pb->SetValue( 0.27f );

	Input::ChibiGFX GwenInput;
	GwenInput.Initialize( &pCanvas );

	GwenInput.AddKey(GPIOA, GPIOA_BUTTON, Input::ChibiGFX::KB_TAB);

	bool_t touch = 0, prevtouch = 0;
	while (TRUE) {

	  if (pCanvas.NeedsRedraw()) {
		  pCanvas.RenderCanvas();
	  }
	  prevtouch = touch;
	  if (GwenInput.Touched()) {
		  touch = 1;
	  }
	  else
		  touch = 0;
	  if (prevtouch != touch) // If touch state changed
		  GwenInput.ProcessTouch(touch);

	  GwenInput.ProcessKeys();

	  chThdSleepMilliseconds(10);
	}
}
