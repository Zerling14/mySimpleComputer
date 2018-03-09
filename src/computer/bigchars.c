#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "bigchars.h"

int big_char_plus[2] = {0xFF181800, 0x00001818};//{0x001818FF, 0x18180000};
int big_char_minus[2] = {0xFF000000, 0x00000000};//{0x000000FF, 0x00000000};
int big_char_spiral[2] = {0xFF1111F1, 0x009F9090};//{0x8F8888FF, 0x0909F900};
int big_char_num[16][2] = {
	/*
	{0x3CC3C3DB, 0xDBC3C33C},//0
	{0x18781818, 0x1818187E},//1
	{0x3CC30306, 0x1860C0FF},//2
	{0x3CC3031C, 0x1C03C33C},//3
	{0x0E1E3666, 0xC6FF060F},//4
	{0xFFC0C0FC, 0x0303C33C},//5
	{0x3E60C0FE, 0xC3C3C37E},//6
	{0xFFC3060C, 0x18181818},//7
	{0x3CC3C37E, 0xC3C3C37E},//8
	{0x7EC3C37F, 0x0303063C},//9
	{0x183C66C3, 0xC3FFC3C3},//A
	{0xFE63637E, 0x636363FE},//B //0111 7
	{0x7EC3C3C0, 0xC0C3C37E},//C //1100 C
	{0xFC666363, 0x636366FC},//D //0110 6
	{0xFF61607E, 0x606061FF},//E //0011 3
	{0xFF61607E, 0x606060F0}};*///F//1110 E
	
	{0xDBC3C33C, 0x3CC3C3DB},//0
	{0x18181E18, 0x7E181818},//1
	{0x60C0C33C, 0xFF030618},//2
	{0x38C0C33C, 0x3CC3C038},//3
	{0x666C7870, 0xF060FF63},//4
	{0x3F0303FF, 0x3CC3C0C0},//5
	{0x7F03067C, 0x7EC3C3C3},//6
	{0x3060C3FF, 0x18181818},//7
	{0x7EC3C33C, 0x7EC3C3C3},//8
	{0xFEC3C37E, 0x3C60C0C0},//9
	{0xC3663C18, 0xC3C3FFC3},//10
	{0x7EC6C67F, 0x7FC6C6C6},//11
	{0x03C3C37E, 0x7EC3C303},//12
	{0xC6C6663F, 0x3F66C6C6},//13
	{0x7E0686FF, 0xFF860606},//14
	{0x7E0686FF, 0x0F060606}};//15

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
	if (!big) {
		return -1;
	}
	mt_setfgcolor(clr1);
	mt_setbgcolor(clr2);
	int tmp_y = 0;
	for (int k = 0; k < 2; k++) {
		for (int i = 0; i < 32; i++) {
			if (i % 8 == 0) {
				tmp_y += 1;
				mt_gotoXY(x, y + tmp_y);
			}
			if (big[k] & 1 << i) {
				char buff[2];
				buff[0] = ACS_CKBOARD;
				buff[1] = 0;
				bc_printA(buff);
			} else {
				printf(" ");
			}
		}
	}
	mt_resetcolor();
	printf("\n");
	return 0;
}
int bc_setbigcharpos(int *big, int x, int y, int value)
{
	int pos;

	if ((x < 0) || (y < 0) || (x > 7) || (y > 7) || (value < 0) || (value > 1)) {
		return -1;
	}
	if (y <= 3) {
		pos = 0;
	} else {
		pos = 1;
	}
	y = y % 4;
	if (value == 0) {
		big[pos] &= ~(1 << (y * 8 + x));
	}
	else {
		big[pos] |= 1 << (y * 8 + x);
	}

	return 0;
}
int bc_getbigcharpos(int *big, int x, int y, int *value)
{
	int pos;

	if ((x < 0) || (y < 0) || (x > 7) || (y > 7)) {
		return -1;
	}
	if (y <= 3) {
		pos = 0;
	} else {
		pos = 1;
	}
	y = y % 4;
	*value = (big[pos] >> (y*8 + x)) & 1;

	return 0;
}
int bc_bigcharwrite(int fd, int *big, int count)
{
	if (!big) {
		return -1;
	}
	while (count > 0) {
		if (write(fd, big, sizeof(int) * 2) == -1) {
			return -1;
		}
		count--;
	}
	return 0;
}
int bc_bigcharread(int fd, int *big, int need_count, int *count)
{
	if (!big || !count) {
		return -1;
	}
	*count = 0;
	while (need_count > 0) {
		int result = read(fd, big, sizeof(int) * 2);
		if (result == -1) {
			return -1;
		}
		if (result != sizeof(int) * 2) {
			return 0;
		}
		need_count--;
		*count = *count + 1;
	}
	return 0;
}

int bc_bigprintint(int x, int y, enum colors clr1, enum colors clr2, int value)
{
	for (int i = 3; i >= 0; i--) {
		bc_printbigchar(big_char_num[(value >> i * 4) & 0xF],
			x + 9 * (3 - i), y, clr1, clr2);
		//printf("%x\n", (value >> i * 4) & 0xF);
	}
	return 0;
}
