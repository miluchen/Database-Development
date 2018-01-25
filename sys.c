/*
 * this source file contains some system-specific functions that are isolated
 * to separate the compiler, operating system, and hardward dependencies from
 * the rest of the software
 */

#include <stdio.h>
#include "cdata.h"
#include "keys.h"

/****** get a keyboard character ******/
int get_char() {
	int c;

	if (!(c = getch()))
		c = getch() | 128;
	return c & 255;
}

/****** write a character to the screen ******/
void put_char(int c) {
	switch (c) {
		case FWD:
			printf("\033[C");
			break;
		case UP:
			printf("\033[A");
			break;
		default:
			putchar(c);
	}
	fflush(stdout);
}

/****** set the cursor position ******/
void cursor(int x, int y) {
	printf("\033[%d;%dH", y+1, x+1);
	fflush(stdout);
}

/****** clear the screen ******/
int screen_displayed = 0;
void clear_screen() {
	screen_displayed = 0;
	printf("\033[H\033[J");
	fflush(stdout);
}
