#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "myterm.h"
#include "interface.h"
#include "memory.h"
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

char log_buff[1024] = {};
int max_log_lines = 6;

int line_count(char *str)
{
	int lines_num = 0;
	for (int i = 0; i < strlen(str); ++i)
	{
		if (str[i] == '\n')
		{
			++lines_num;
		}
	}
	return lines_num;
}

void move_log(char *str)
{
	int next_line_pos = 0;
	for (int i = 0; i < strlen(str); ++i)
	{
		if (str[i] == '\n')
		{
			next_line_pos = i;
			break;
		}
	}
	int old_str_size = strlen(str);
	for (int i = next_line_pos + 1; i < strlen(str); ++i)
	{
		str[i - (next_line_pos + 1)] = str[i];
	}
	str[old_str_size - next_line_pos - 1] = 0;
}

void clear_log_place(int x, int y, char *str)
{
	int line_num = line_count(str);
	if (line_num == 0)
	{
		return;
	}
	int max_line_lenght = 0;
	int last_line_end = 0;
	for (int i = 0; i < strlen(str); ++i)
	{
		if ((str[i] == '\n') || (i == (strlen(str) - 1)))
		{
			if (i - last_line_end > max_line_lenght)
			{
				max_line_lenght = i - last_line_end;
			}
			last_line_end = i + 1;
		}
	}
	
	
	char *buf = calloc(max_line_lenght, sizeof(char));
	memset(buf, ' ', max_line_lenght);
	for (int i = 0; i < line_num; ++i)
	{
		mt_gotoXY(x, y + i);
		printf("%s", buf);
	}
}

int mi_printlog(int x, int y)
{
	

	if (line_count(log_buff) > max_log_lines) {
		clear_log_place(x, y, log_buff);
		move_log(log_buff);
		//log_buff[0] = 0;
		//mt_clrsrc();
	}
	mt_gotoXY(x, y);
	printf("Logs[%lu]:\n%s", strlen(log_buff), log_buff);
	return 0;
}

int mi_printdecoderesult(int x, int y, int address)
{
	bc_box(x, y, 19, 3);
	mt_gotoXY(x + 2, y);
	puts("decodeResult");
	mt_gotoXY(x + 5, y + 1);
	int value;
	if (sc_memoryGet(address, &value)) {
		return 1;
	}
	int command;
	int operand;
	if (sc_commandDecode(value, &command, &operand)) {
		printf("nan:nan");
	} else {
		printf("%02d:%02d", command, operand);
	}
	return 0;
}

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
	if (mi_printinstrutioncounter(MEMORY_W + 1, 4, insp_reg)) {
		return 1;
	}
	if (mi_printdecoderesult(MEMORY_W + 1, 7, select_x + select_y * 10)) {
		return 1;
	}
	if (mi_printaccumulator(MEMORY_W + 1, 1, acc_reg)) {
		return 1;
	}
	if (mi_printselectedmemory(BIGNUM_X, BIGNUM_Y, select_x + select_y * 10)) {
		return 1;
	}
	if (mi_printlog(1, BIGNUM_Y + BIGNUM_H)) {
		return 1;
	}
	//mt_gotoXY(1, MEMORY_H + KEYS_H + 1);
	return 0;
}

int mi_printselectedmemory(int x, int y, int address)
{
	bc_box(x, y, BIGNUM_W, BIGNUM_H);
	int value;
	if (sc_memoryGet(address, &value)) {
		return 1;
	}
	if ((value & 0x4000) == 0) {
		bc_printbigchar(big_char_plus, x + 1, y, none, 3);
		bc_bigprintint(x + 10, y, none, 3, value & 0x3FFF);
	} else {
		bc_printbigchar(big_char_minus, x + 1, y, none, 3);
		bc_bigprintint(x + 10, y, none, 3, value & 0x3FFF);
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
			if ((val & 0x4000) == 0) {
				printf("+%04X", val & 0x3FFF);
			} else {
				printf("-%04X", val & 0x3FFF);
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
	//for (int i = 0; i < 5; i++) {
	int val = 0;
	sc_regGet(0, &val);
	printf("C%d", val);
	sc_regGet(1, &val);
	printf("D%d", val);
	sc_regGet(2, &val);
	printf("M%d", val);
	sc_regGet(3, &val);
	printf("I%d", val);
	sc_regGet(4, &val);
	printf("E%d", val);
	//}
	
	return 0;
}

int mi_printinstrutioncounter(int x, int y, int val)
{
	bc_box(x, y, 19, 3);
	mt_gotoXY(x + 2, y);
	puts("instCount");
	mt_gotoXY(x + 5, y + 1);
	if (val >= 0) {
		printf("+%04X", val);
	} else {
		printf("-%04X", 0 - val);
	}
	return 0;
}

int mi_printaccumulator(int x, int y, int val)
{
	bc_box(x, y, 19, 3);
	mt_gotoXY(x + 2, y);
	puts("accumulator");
	mt_gotoXY(x + 5, y + 1);
	if ((val & 0x4000) == 0) {
		printf("+%04X", val & 0x3FFF);
	} else {
		printf("-%04X", val & 0x3FFF);
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
