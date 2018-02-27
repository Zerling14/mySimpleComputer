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

extern int big_char_plus[2];
extern int big_char_minus[2];
extern int big_char_spiral[2];
extern int big_char_num[16][2];

int bc_printA(char *str);
int bc_box(int x1, int y1, int w, int h);
int bc_printbigchar(int *big, int x, int y, enum colors clr1, enum colors clr2);
int bc_setbigcharpos(int *big, int x, int y, int value);
int bc_getbigcharpos(int *big, int x, int y, int *value);
int bc_bigcharwrite(int fd, int *big, int count);
int bc_bigcharread(int fd, int *big, int need_count, int *count);
int bc_bigprintint(int x, int y, enum colors clr1, enum colors clr2, int value);

#endif
