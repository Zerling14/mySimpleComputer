#include <stdio.h>
#include "myterm.h"
#include "interface.h"
#include "controldevice.h"
#include "bigchars.h"

#define MEMORY_X 1
#define MEMORY_Y 1
#define MEMORY_H 12
#define MEMORY_W (2 + 10 * 6)

#define FLAGS_X (MEMORY_W + 1)
#define FLAGS_Y 10

#define KEYS_X (5 + 7 * 6)
#define KEYS_Y (MEMORY_H + 1)
#define KEYS_H 10

#define BIGNUM_X 1
#define BIGNUM_Y (KEYS_H + 3)
#define BIGNUM_H (KEYS_H)
#define BIGNUM_W (KEYS_X - 1)

int mi_printinterface(int select_x, int select_y)
{
	if (mi_printmemory(MEMORY_X, MEMORY_Y, select_x, select_y)) {
		return 1;
	}
	if (mi_printflags(FLAGS_X, FLAGS_Y)) {
		return 1;
	}
	if (mi_printkeytooltip(KEYS_X, KEYS_Y)) {
		return 1;
	}
	if (mi_printselectedmemory(BIGNUM_X, BIGNUM_Y, select_x + select_y * 10)) {
		return 1;
	}
	
	mt_gotoXY(1, MEMORY_H + KEYS_H + 1);
	return 0;
}

int mi_printselectedmemory(int x, int y, int address)
{
	bc_box(x, y, BIGNUM_W, BIGNUM_H);
	int value;
	if (sc_memoryGet(address, &value)) {
		return 1;
	}
	if (value >= 0) {
		bc_printbigchar(big_char_plus, x + 1, y, -1, 7);
		bc_bigprintint(x + 10, y, -1, 7, value);
	} else {
		bc_printbigchar(big_char_minus, x + 1, y, -1, 7);
		bc_bigprintint(x + 10, y, -1, 7, 0 - value);
	}
	return 0;
}

int mi_printmemory(int x, int y, int select_x, int select_y)
{
	bc_box(x, y, MEMORY_W, MEMORY_H);
	mt_gotoXY(x + 2, y);
	puts("Memory");
	for (int i = 0; i < 10; i++) {
		mt_gotoXY(x + 1, y + i + 1);
		for (int j = 0; j < 10; j++) {
			
			int val;
			if (sc_memoryGet(i * 10 + j, &val)) {
				return 1;
			}
			if (j == select_x && i == select_y) {
				mt_setfgcolor(blue);
			} else {
				mt_resetcolor();
			}
			if (val >= 0) {
				printf("+%04X", val);
			} else {
				printf("-%04X", 0 - val);
			}
			if (j < 9) {
				printf(" ");
			}
			mt_resetcolor();
		}
	}
	return 0;
}

int mi_printflags(int x, int y)
{
	bc_box(x, y, 19, 3);
	mt_gotoXY(x + 2, y);
	puts("Flags");
	mt_gotoXY(x + 5, y + 1);
	for (int i = 0; i < 5; i++) {
		int val = 0;
		sc_regGet(i, &val);		
		printf("%d ", val);
	}
	
	return 0;
}

int mi_printkeytooltip(int x, int y)
{
	bc_box(x, y, 35, KEYS_H);
	mt_gotoXY(x + 2, y);
	puts("Keys");
	mt_gotoXY(x + 1, y + 1);
	puts("l - load");
	mt_gotoXY(x + 1, y + 2);
	puts("s - save");
	mt_gotoXY(x + 1, y + 3);
	puts("r - run");
	mt_gotoXY(x + 1, y + 4);
	puts("t - step");
	mt_gotoXY(x + 1, y + 5);
	puts("i - reset");
	mt_gotoXY(x + 1, y + 6);
	puts("F5 - accumulator");
	mt_gotoXY(x + 1, y + 7);
	puts("F6 - instructionCounter");
	
	return 0;
}
