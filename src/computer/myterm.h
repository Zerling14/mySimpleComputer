#ifndef MYTERM_H
#define MYTERM_H
/*
clear_screen=\E[H\E[J
cursor_address=\E[%i%p1%d;%p2%dH
cursor_visible=\E[?25h\E[?8c
cursor_invisible=\E[?25l\E[?1c
set_a_background=\E[4%p1%dm
set_a_foreground=\E[3%p1%dm

Color       #define       Value       RGB
black     COLOR_BLACK       0     0, 0, 0
red       COLOR_RED         1     max,0,0
green     COLOR_GREEN       2     0,max,0
yellow    COLOR_YELLOW      3     max,max,0
blue      COLOR_BLUE        4     0,0,max
magenta   COLOR_MAGENTA     5     max,0,max
cyan      COLOR_CYAN        6     0,max,max
white     COLOR_WHITE       7     max,max,max
*/

enum colors {
	none = -1,
	black = 0,
	red,
	green,
	yellow,
	blue,
	magenta,
	cyan,
	white
};

int mt_clrsrc();
int mt_gotoXY(int x, int y);
int mt_get_screensize(int *rows, int *cols);
int mt_setfgcolor(enum colors clr);
int mt_setbgcolor(enum colors clr);
int mt_resetcolor();
int mt_resetcursor();

#endif

