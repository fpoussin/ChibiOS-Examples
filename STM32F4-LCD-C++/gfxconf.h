/*
 * gfxconf.h
 *
 *  Created on: Apr 4, 2013
 *      Author: fpoussin
 */

#ifndef _GFXCONF_H
#define _GFXCONF_H

/*===========================================================================*/
/* GDISP driver related settings.                                              */
/*===========================================================================*/

#define GDISP_SCREEN_WIDTH 480
#define GDISP_SCREEN_HEIGHT 272

#define GDISP_USE_FSMC
//#define GDISP_USE_GPIO

#define GDISP_USE_DMA
#define GDISP_DMA_STREAM STM32_DMA2_STREAM6

#if defined(GDISP_USE_GPIO)

    #define GDISP_CMD_PORT GPIOC
    #define GDISP_DATA_PORT GPIOD

    #define GDISP_CS 0
    #define GDISP_RS 1
    #define GDISP_WR 2
    #define GDISP_RD 3
#endif

/* GFX sub-systems to turn on */
#define GFX_USE_GDISP			TRUE
#define GFX_USE_GWIN			TRUE
#define GFX_USE_GEVENT			TRUE
#define GFX_USE_GTIMER			TRUE
#define GFX_USE_GINPUT			TRUE

/* Features for the GDISP sub-system. */
#define GDISP_NEED_VALIDATION	TRUE
#define GDISP_NEED_CLIP		TRUE
#define GDISP_NEED_TEXT		TRUE
#define GDISP_NEED_CIRCLE		TRUE
#define GDISP_NEED_ELLIPSE		FALSE
#define GDISP_NEED_ARC			FALSE
#define GDISP_NEED_SCROLL		FALSE
#define GDISP_NEED_PIXELREAD	FALSE
#define GDISP_NEED_CONTROL		TRUE
#define GDISP_NEED_MULTITHREAD	TRUE
#define GDISP_NEED_ASYNC		FALSE
#define GDISP_NEED_MSGAPI		FALSE

#define GINPUT_NEED_MOUSE		TRUE
#define GINPUT_MOUSE_USE_CUSTOM_BOARD TRUE
#define GINPUT_MOUSE_YX_INVERTED TRUE

/* Features for the GWIN sub-system. */
#define GWIN_NEED_GRAPH		TRUE
#define GWIN_NEED_CONSOLE		TRUE

/* Builtin Fonts */
#define GDISP_INCLUDE_FONT_SMALL			FALSE
#define GDISP_INCLUDE_FONT_LARGER			FALSE
#define GDISP_INCLUDE_FONT_UI1				FALSE
#define GDISP_INCLUDE_FONT_UI2				TRUE
#define GDISP_INCLUDE_FONT_LARGENUMBERS	FALSE

#endif /* _GFXCONF_H */
