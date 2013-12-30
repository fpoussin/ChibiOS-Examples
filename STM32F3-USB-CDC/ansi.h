/*
 * ansi.h
 *
 *  Created on: Nov 20, 2013
 *      Author: Jed Frey
 */

#include "ch.h"
#include "hal.h"
#include "chprintf.h"
#include "shell.h"

#ifndef ANSI_H_
#define ANSI_H_

void ansiXY(BaseSequentialStream *chp, unsigned int x, unsigned int y);
void ansiClear(BaseSequentialStream *chp);
void cmd_ansiColorTest(BaseSequentialStream *chp, int argc, char *argv[]);
void cmd_echo(BaseSequentialStream *chp, int argc, char *argv[]);
void cmd_getKey(BaseSequentialStream *chp, int argc, char *argv[]);
void cmd_ExtendedAscii(BaseSequentialStream *chp, int argc, char *argv[]);
void cmd_box(BaseSequentialStream *chp, int argc, char *argv[]);
void cmd_draw(BaseSequentialStream *chp, int argc, char *argv[]);
void cmd_template(BaseSequentialStream *chp, int argc, char *argv[]);
#endif /* ANSI_H_ */
