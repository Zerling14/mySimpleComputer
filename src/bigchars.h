#ifndef BIGCHARS_H
#define BIGCHARS_H
#include "myterm.h"

//a f g i j k l m n o p q r s t u v w x y z { | } ~
//▒ ° ± ␋ ┘ ┐ ┌ └ ┼ ⎺ ⎻ ─ ⎼ ⎽ ├ ┤ ┴ ┬ │ ≤ ≥ π ≠ £ ·

#define ACS_ULCORNER 'l'
#define ACS_LLCORNER 'm'
#define ACS_URCORNER 'k'
#define ACS_LRCORNER 'j'
#define ACS_HLINE 'q'
#define ACS_VLINE 'x'
#define ACS_CKBOARD 'a'

int bc_printA(char *str);
int bc_box(int x1, int y1, int w, int h);
int bc_printbigchar(int *big, int x, int y, enum colors clr1, enum colors clr2);
int bc_setbigcharpos(int *big, int x, int y, int value);
int bc_getbigcharpos(int *big, int x, int y, int *value);

#endif
