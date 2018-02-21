#include <stdio.h>
#include <termios.h>
#include <sys/ioctl.h>
#include "myterm.h"

int mt_clrsrc() {
	if (printf("\E[H\E[J") < 0) {
		return -1;
	}
	return 0;
}
int mt_gotoXY(int x, int y) {
	if (printf("\E[%d;%dH", y, x) < 0) {
		return -1;
	}
	return 0;
}
int mt_get_screensize(int *rows, int *cols) {
	if (!rows || !cols) {
		return -1;
	}
	struct winsize ws;
	if (!ioctl(1, TIOCGWINSZ, &ws)) {
		*rows = ws.ws_row;
		*cols = ws.ws_col;
	} else {
		return -1;
	}
	return 0;
}
int mt_setfgcolor(enum colors clr) {
	if (printf("\E[4%dm", clr) < 0) {
		return -1;
	}
	return 0;
}
int mt_setbgcolor(enum colors clr) {
	if (printf("\E[3%dm", clr) < 0) {
		return -1;
	}
	return 0;
}

int mt_resetcolor() {
	if (printf("\E[0m") < 0) {
		return -1;
	}
	return 0;
}

int mt_resetcursor() {
	if (printf("\E[H") < 0) {
		return -1;
	}
	return 0;
}
