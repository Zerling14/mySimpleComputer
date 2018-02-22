#include <stdio.h>
#include <stdlib.h>
#include "bigchars.h"

int bc_printA(char *str)
{
	if (!str) {
		return -1;
	}
	printf("\E(0%s\E(B", str); //\E[11m not work
	return 0;
}

int bc_box(int x, int y, int w, int h)
{
	if (x < 0 || y < 0 || w < 0 || h < 0) {
		return -1;
	}
	char *buff = malloc(w + 3);
	if (!buff) {
		return -1;
	}
	
	for (int i = 0; i < h; i++) {
		int str_count = 1;
		if (i == 0) {
			buff[0] = ACS_ULCORNER;
			for (int j = 0; j < w - 2; j++) {
				buff[str_count++] = ACS_HLINE;
			}
			buff[str_count++] = ACS_URCORNER;
		} else if (i == h - 1) {
			buff[0] = ACS_LLCORNER;
			for (int j = 0; j < w - 2; j++) {
				buff[str_count++] = ACS_HLINE;
			}
			buff[str_count++] = ACS_LRCORNER;
		} else {
			buff[0] = ACS_VLINE;
			for (int j = 0; j < w - 2; j++) {
				buff[str_count++] = ' ';
			}
			buff[str_count++] = ACS_VLINE;
		}
		mt_gotoXY(x, y + i);
		buff[str_count++] = 0;
		bc_printA(buff);
	}
	free(buff);
	return 0;
}

int bc_printbigchar(int *big, int x, int y, enum colors clr1, enum colors clr2)
{
	mt_gotoXY(x, y);
	int tmp_y = 0;
	for (int i = 0; i < 32; i++) {
		if (i % 8 == 0 && i != 0) {
			tmp_y += 1;
			mt_gotoXY(x, y + tmp_y);
		}
		if (big[0] & 1 << (31 - i)) {
			char buff[2];
			buff[0] = ACS_CKBOARD;
			buff[1] = 0;
			bc_printA(buff);
		} else {
			printf(" ");
		}
	}
	printf("\n");
	return 0;
}
int bc_setbigcharpos(int *big, int x, int y, int value)
{
	return 0;
}
int bc_getbigcharpos(int *big, int x, int y, int *value)
{
	return 0;
}
