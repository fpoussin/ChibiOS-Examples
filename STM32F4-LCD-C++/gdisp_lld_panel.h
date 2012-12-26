/*
    ChibiOS/GFX Copyright (C) 2012
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
#define GDISP_SCREEN_WIDTH 480
#define GDISP_SCREEN_HEIGHT 272

#define SCREEN_FPS 60ULL

#define SCREEN_HSYNC_BACK_PORCH		2LL
#define SCREEN_HSYNC_FRONT_PORCH	2ULL
#define SCREEN_HSYNC_PULSE			41ULL

#define SCREEN_VSYNC_BACK_PORCH		2ULL
#define SCREEN_VSYNC_FRONT_PORCH	2ULL
#define SCREEN_VSYNC_PULSE			10ULL

#define	SCREEN_HSYNC_PERIOD	(SCREEN_HSYNC_PULSE + SCREEN_HSYNC_BACK_PORCH + GDISP_SCREEN_WIDTH  + SCREEN_HSYNC_FRONT_PORCH)
#define	SCREEN_VSYNC_PERIOD	(SCREEN_VSYNC_PULSE + SCREEN_VSYNC_BACK_PORCH + GDISP_SCREEN_HEIGHT + SCREEN_VSYNC_FRONT_PORCH)

#define SCREEN_PCLK	(SCREEN_HSYNC_PERIOD * SCREEN_VSYNC_PERIOD * SCREEN_FPS)
#define GDISP_FPR		((SCREEN_PCLK * 1048576)/100000000)

#endif
/** @} */
