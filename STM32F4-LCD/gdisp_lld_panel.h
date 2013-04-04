/*
    ChibiOS/GFX Copyright (C) 2012, 2013
			Joel Bodenmann aka Tectu <joel@unormal.org>

    This file is part of ChibiOS/GFX.

    ChibiOS/GFX is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    ChibiOS/GFX is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/**
 * @file    SSD1963/gdisp_lld_panel.h
 * @brief   TFT LCD panel properties.
 *
 * @addtogroup GDISP
 * @{
 */

#ifndef _GDISP_LLD_PANEL_H
#define _GDISP_LLD_PANEL_H

/* LCD panel specs */

/* The timings need to follow the datasheet for your particular TFT/LCD screen (the actual screen, not the controller)
*** Datasheets normally use a specific set of timings and acronyms, their value refers to the number of pixel clocks
** Non-display periods refer to pulses/timings that occur before or after the timings that actually put pixels on the screen
** Display periods refer to pulses/timings that directly put pixels on the screen
HDP: Horizontal Display Period, normally the width - 1
HT: Horizontal Total period (display + non-display)
HPS: non-display period between the start of the horizontal sync (LLINE) signal and the first display data
LPS: horizontal sync pulse (LLINE) start location in pixel clocks
HPW: Horizontal sync Pulse Width
VDP: Vertical Display period, normally height - 1
VT: Vertical Total period (display + non-display)
VPS: non-display period in lines between the start of the frame and the first display data in number of lines
FPS: vertical sync pulse (LFRAME) start location in lines. 
VPW: Vertical sync Pulse Width

*** Here's how to convert them:
HPS = SCREEN_HSYNC_PULSE + SCREEN_HSYNC_BACK_PORCH
HT - HPS = GDISP_SCREEN_WIDTH + SCREEN_HSYNC_FRONT_PORCH
=> 	SCREEN_HSYNC_FRONT_PORCH = ( HT - HPS ) - GDISP_SCREEN_WIDTH
	SCREEN_HSYNC_PULSE = HPW
	SCREEN_HSYNC_BACK_PORCH = HPS - HPW
	SCREEN_HSYNC_PERIOD = HT
	
VPS = SCREEN_VSYNC_PULSE + SCREEN_VSYNC_BACK_PORCH
VT - VPS = GDISP_SCREEN_HEIGHT + SCREEN_VSYNC_FRONT_PORCH
=>	SCREEN_VSYNC_FRONT_PORCH = ( VT - VPS ) - GDISP_SCREEN_HEIGHT
	SCREEN_VSYNC_PULSE = VPW
	SCREEN_VSYNC_BACK_PORCH = VPS - LPS
	SCREEN_VSYNC_PERIOD = VT
*/

#define SCREEN_FPS 60ULL

//The following values are for a 4.3" TFT LCD

#define GDISP_SCREEN_WIDTH 480
#define GDISP_SCREEN_HEIGHT 272

#define SCREEN_HSYNC_BACK_PORCH		2
#define SCREEN_HSYNC_FRONT_PORCH	2
#define SCREEN_HSYNC_PULSE			41

#define SCREEN_VSYNC_BACK_PORCH		2
#define SCREEN_VSYNC_FRONT_PORCH	2
#define SCREEN_VSYNC_PULSE			10

#define	SCREEN_HSYNC_PERIOD	(SCREEN_HSYNC_PULSE + SCREEN_HSYNC_BACK_PORCH + GDISP_SCREEN_WIDTH  + SCREEN_HSYNC_FRONT_PORCH)
#define	SCREEN_VSYNC_PERIOD	(SCREEN_VSYNC_PULSE + SCREEN_VSYNC_BACK_PORCH + GDISP_SCREEN_HEIGHT + SCREEN_VSYNC_FRONT_PORCH)

#define SCREEN_PCLK	(SCREEN_HSYNC_PERIOD * SCREEN_VSYNC_PERIOD * SCREEN_FPS)
#define GDISP_FPR		((SCREEN_PCLK * 1048576)/100000000)

#endif
/** @} */

