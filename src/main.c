#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "controldevice.h"
#include "debug.h"
#include "myterm.h"
#include "interface.h"
#include "bigchars.h"

void print_test_lab1()
{
	sc_memoryInit();
	// set in memory multiplication table
	printf("TEST MEMORY OUTPUT\n");
	for (int i = 0; i < 10; i++) { 
		for (int j = 0; j < 10; j++) {
			sc_memorySet(i * 10 + j, i * j);
		}
	}
	// set in memory multiplication table
	sm_printMemory();
	
	// save table
	printf("TEST MEMORY SAVE\n");
	if (sc_memorySave("mul_table.bin")) {
		printf("\n error memory save \n");
	}
	// clear memory
	sc_memoryInit();
	sm_printMemory();
	
	// load table
	printf("TEST MEMORY LOAD\n");
	if (sc_memoryLoad("mul_table.bin")) {
		printf("\n error memory load \n");
	}
	system("clear");
	sm_printMemory();

	sc_regInit();
	// check flags
	printf("TEST MEMORY SET\n");
	sm_printFlags();
	sc_memorySet(100, 9999);
	sm_printFlags();
	
	sc_regSet(MF, 0);
	printf("TEST MEMORY GET\n");
	int val1 = 0;
	sm_printFlags();
	sc_memoryGet(100, &val1);
	printf("%d\n", val1);
	sm_printFlags();
	
	printf("\n");
	val1 = 0;
	sm_printFlags();
	sc_memoryGet(99, &val1);
	printf("%d\n", val1);
	sm_printFlags();
	
	//check command encode/decode
	printf("TEST COMMAND ENCODE\n");
	int val2 = 0;
	if (sc_commandEncode(76, 127, &val2)) {
		printf("\n error memory encode \n");
	}
	printf("command:76, operand:127 encoded:%X\n", val2);
	int command = 0;
	int operand = 0;
	sc_commandDecode(val2, &command, &operand);
	printf("encoded:%X, command:%d operand:%d\n", val2, command, operand);
}

void print_test_lab2()
{
	mt_clrsrc();
	mt_gotoXY(10, 5);
	mt_setbgcolor(red);
	mt_setfgcolor(black);
	printf("Александр");
	
	mt_gotoXY(11, 6);
	mt_setbgcolor(green);
	mt_setfgcolor(white);
	printf("ИВ-621");
	
	int rows, cols;
	mt_get_screensize(&rows, &cols);
	mt_gotoXY(6, 7);
	mt_resetcolor();
	printf("rows: %d cols: %d", rows, cols);	
	
	mt_resetcursor();
	mt_resetcolor();
}

void print_test2_lab2()
{
	sc_memoryInit();
	// set in memory multiplication table
	for (int i = 0; i < 10; i++) { 
		for (int j = 0; j < 10; j++) {
			sc_memorySet(i * 10 + j, i * j * pow(-1, i + j));
		}
	}

	mt_clrsrc();
	mi_printinterface();
	int rows, cols;
	mt_get_screensize(&rows, &cols);
	printf("rows: %d cols: %d\n", rows, cols);
}

void print_test_lab3()
{
	mt_clrsrc();
	//printf("afgijklmnopqrstuvwxyz{|}~\n");
	//bc_printA(ACS_CKBOARD"\n");
	//printf("\E(0%s\E(B", str);
	bc_box(1, 1, 5, 2);
}

int main()
{
	//print_test_lab1();
	//print_test_lab2();
	//print_test2_lab2();
	print_test_lab3();
	return 0;
}
