#include "Gwen/Gwen.h"
#include "Gwen/Input/ChibiGFX.h"
#include "Gwen/Renderers/ChibiGFX.h"
#include "Gwen/Skins/Light.h"
#include "Gwen/Controls/Canvas.h"
#include "Gwen/Controls/Button.h"
#include "Gwen/Controls/ProgressBar.h"
#include "gui.h"

using namespace Gwen;

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
	pButton1->SetPos( 10, 10 );
	pButton1->SetBounds( 10, 10, 50, 50 );
	pButton1->SetText( "Button1" );
	//pButton1->onPress.Add( pCanvas, &onPress );

	//Controls::Button* pButton2 = new Controls::Button( &pCanvas );
	//pButton2->SetPos( 60, 0 );
	//pButton2->SetBounds( 0, 0, 50, 50 );
	//pButton2->SetText( "Hello2" );
	//pButton2->onPress.Add( &pCanvas, &onPress );

	Gwen::Controls::ProgressBar* pb = new Gwen::Controls::ProgressBar( &pCanvas );
	pb->SetBounds(110, 20, 200, 20);
	pb->SetValue( 0.27f );

	Input::ChibiGFX GwenInput;
	GwenInput.Initialize( &pCanvas );

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
