/*
 * ansi.c
 *
 *  Created on: Nov 20, 2013
 *      Author: Jed Frey
 */

#include "ansi.h"

void ansiXY(BaseSequentialStream *chp, unsigned int x, unsigned int y) {
  chprintf(chp, "\e[%d;%dH",y,x);
}

void ansiClear(BaseSequentialStream *chp) {
	chprintf(chp, "\e[2J\e[H");
}

void cmd_ansiColorTest(BaseSequentialStream *chp, int argc, char *argv[]) {
	(void)argv;
	(void)argc;
	unsigned int i,j, k;
	for (i=0;i<8;i++) {
		for (k=0;k<8;k++) {
			for (j=0;j<8;j++) {
				chprintf(chp, "\e[%dm",i+30);
				chprintf(chp, "\e[%dm",j);
				chprintf(chp, "\e[%dm",k+40);
				chprintf(chp, "Hello");
				chprintf(chp, "\x1b[0m ");
			}
			chprintf(chp,"\r\n");
		}
	}
}

void cmd_echo(BaseSequentialStream *chp, int argc, char *argv[]) {
	(void)argv;
	(void)argc;
	chprintf(chp,"This command echos any key press to the console as a character,\r\n");
	chprintf(chp,"hex, and decimal equivalents. The ONLY way to exit this mode is\r\n");
	chprintf(chp,"by resetting the MCU.\r\nPress any key to exit or Space to continue\r\n");
	uint8_t i=0,c;

	if (chSequentialStreamRead(chp, &c, 1) == 0) {
		return;
	}
	if (c != 32) {
		chprintf(chp, "Exiting...\r\n");
		return;
	}
	chprintf(chp, "\r\nReset MCU To Exit\r\n");

	chprintf(chp, "%5s%5s%5s\r\n","Char","Hex","Dec");
	while (TRUE) {
		if (chSequentialStreamRead(chp, &c, 1) == 0) {
			return;
		}
		if (i++==25) {
			chprintf(chp, "\r\n%5s%5s%5s\r\n","Char","Hex","Dec");
			i=0;
		}
		if ((c>31)&(c<127)) {
			chprintf(chp, "%5c 0x%2x%5u\r\n",c,c,c);
		} else {
			// Unprintable characters throw off the formatting
			chprintf(chp, "      0x%2x%5u\r\n",c,c);
		}
	}
}

void cmd_getKey(BaseSequentialStream *chp, int argc, char *argv[]) {
	(void)argv;
	(void)argc;
	uint8_t c;
	while (TRUE) {
		if (chSequentialStreamRead(chp, &c, 1) == 0) {
			return;
		}
		if ((c>=1)&(c<=26)) {
			switch (c) {
				case 9:
					chprintf(chp, "^-I / Tab\r\n");
					break;
				case 13:
					chprintf(chp, "^-M / Enter\r\n");
					break;
				default:
					chprintf(chp, "^-%c\r\n",c+64);
			}
			continue;
		} else if((c>=32)&(c<=126)) {
			chprintf(chp, "%c\r\n",c);
		} else if (c == 27) {
			if (chSequentialStreamRead(chp, &c, 1) == 0)
				return;
			if (c==91) {
				if (chSequentialStreamRead(chp, &c, 1) == 0)
					return;
				switch (c) {
					case 65:
						chprintf(chp, "\t**UP**\r\n");
						continue;
					case 66:
						chprintf(chp, "\t**DOWN**\r\n");
						continue;
					case 67:
						chprintf(chp, "\t**RIGHT**\r\n");
						continue;
					case 68:
						chprintf(chp, "\t**LEFT**\r\n");
						continue;
					default:
						continue;
				}
			} else {
				continue;
			}
		}
	}
}

void cmd_ExtendedAscii(BaseSequentialStream *chp, int argc, char *argv[]) {
	(void)argv;
	(void)argc;
	unsigned int i,j=0;
	for (i=128;i<256;i++) {
		if (j==1) {
			chprintf(chp, "%u\t%c\r\n",i,i);
			j=0;
		} else {
			chprintf(chp, "%u\t %c\r\n",i,i);
			j=1;
		}
	}
}

void drawBox(BaseSequentialStream *chp, unsigned int h, unsigned int w) {
/*
 217 - lower right
 218 - lower left
 191 - upper right
 192 - upper left
 179 - vertical bar
 196 - horizontal bar
*/
	unsigned i,j;
	ansiXY(chp,1,1);
	chprintf(chp, "%c", 218);
	for (i=2;i<w;i++) {
		chprintf(chp, "%c", 196);
	}
	ansiXY(chp,w,1);
	chprintf(chp, "%c", 191);
	for (j=2;j<h;j++) {
		ansiXY(chp,1,j);
		chprintf(chp, "%c", 179);
		ansiXY(chp,w,j);
		chprintf(chp, "%c", 179);
	}
	ansiXY(chp,1,h);
	chprintf(chp, "%c", 192);
	for (i=2;i<w;i++) {
		chprintf(chp, "%c", 196);
	}
	ansiXY(chp,w,h);
	chprintf(chp, "%c", 217);
	chprintf(chp, "\r\n");
}

void drawCursor(BaseSequentialStream *chp, unsigned int x, unsigned int y) {
	ansiXY(chp,x,y);
	/*
	chprintf(chp, "\e[32m");
	chprintf(chp, "\e[2m");
	chprintf(chp, "\e[42m");
	*/
	chprintf(chp, "X");

	//chprintf(chp, "\x1b[0m ");
}
void cmd_box(BaseSequentialStream *chp, int argc, char *argv[]) {
	(void)argv;
	(void)argc;
	ansiClear(chp);
	drawBox(chp,24,80);
}

void cmd_draw(BaseSequentialStream *chp, int argc, char *argv[]) {
	(void)argc;
	(void)argv;
	unsigned char c;
	unsigned int h=24,w=80,x=2,y=2;
	static unsigned int h_min=10,h_max=50,w_min=10,w_max=80;

	ansiClear(chp);
	drawBox(chp,h,w);
	drawCursor(chp,x,y);
	ansiXY(chp,1,h+1);
	while (TRUE) {
		if (chSequentialStreamRead(chp, &c, 1) == 0) {
			return;
		}
		if (c == 4 || c == 3) {
			chprintf(chp,"Killing Threads.");
			ansiClear(chp);
			return;
		}
		if (c == 27) {
			if (chSequentialStreamRead(chp, &c, 1) == 0) {
				return;
			}
			if (c==79) {
				if (chSequentialStreamRead(chp, &c, 1) == 0) {
					return;
				}
				switch (c) {
					case 65:
						if (--h<h_min) {
							h=h_min;
						}
						break;;
					case 66:
						if (++h>h_max) {
							h=h_max;
						}
						break;;
					case 67:
						if (++w>w_max) {
							w=w_max;
						}
						break;
					case 68:
						if (--w<w_min) {
							w=w_min;
						}
						break;
					default:
						continue;
				}
			} else if (c==91) {
				if (chSequentialStreamRead(chp, &c, 1) == 0) {
					return;
				}
				switch (c) {
					case 65:
						if (--y<2) {
							y=2;
						}
						break;;
					case 66:
						if (++y>h-1) {
							y=h-1;
						}
						break;;
					case 67:
						if (++x>w-1) {
							x=w-1;
						}
						break;
					case 68:
						if (--x<2) {
							x=2;
						}
						break;
					default:
						continue;
				}
			} else {
				continue;
			}
		}
		ansiClear(chp);
		drawBox(chp,h,w);
		drawCursor(chp,x,y);
		ansiXY(chp,1,h+1);
		chprintf(chp, "Box Size  : (%d,%d)\r\n",h,w);
		chprintf(chp, "Cursor Pos: (%d,%d)\r\n",x,y);
	}
}


void cmd_template(BaseSequentialStream *chp, int argc, char *argv[]) {
	(void)argv;
	(void)argc;
	(void)chp;
}
