#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/time.h>
#include <signal.h>
#include "controldevice.h"
#include "debug.h"
#include "myterm.h"
#include "interface.h"
#include "bigchars.h"
#include "myreadkey.h"

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
	mi_printinterface(0, 0);
	int rows, cols;
	mt_get_screensize(&rows, &cols);
	printf("rows: %d cols: %d\n", rows, cols);
	
	
}

void print_test_lab4()
{
	mt_clrsrc();
	//printf("afgijklmnopqrstuvwxyz{|}~\n");
	//bc_printA(ACS_CKBOARD"\n");
	//printf("\E(0%s\E(B", str);
	//int tmp[2] = {0b10001111100010001000100010001000, 0b11111111000010010000100111111001};
	//bc_printbigchar(big_char_plus, 1, 0, 0, 0);
	//bc_printbigchar(big_char_1, 1, 8, 0, 0);
	//bc_printbigchar(big_char_spiral, 1, 16, 0, 0);
	int filedesc = open("testfile.txt", O_WRONLY | O_TRUNC | O_CREAT, -1);
	if (filedesc < 0) {
		return;
	}
	bc_bigcharwrite(filedesc, big_char_num[1], 2);
	close(filedesc);

	filedesc = open("testfile.txt", O_RDONLY);
	int count;
	int test[2] = {0, 0};
	bc_bigcharread(filedesc, test, 2, &count);
	bc_printbigchar(test, 1, 0, 0, 1);

	bc_setbigcharpos(test, 0, 2, 1);
	bc_printbigchar(test, 1, 9, red, blue);
	//printf("%d\n", count);
	//bc_bigprintint(1, 0, 0x0123);
	//bc_bigprintint(1, 9, 0x4567);
	//bc_bigprintint(1, 18, 0x89AB);
	//bc_bigprintint(1, 27, 0xCDEF);
}

void print_test_lab5()
{
	enum keys key;
	rk_mytermsave();
	rk_readkey(&key);
	rk_mytermrestor();
	printf("input key %d\n", key);
}

void sighandler(int signo)
{
	printf("Signal %d\n", signo);
	if (signo == 14) {
		raise(SIGUSR1);
	}
}

void print_test_lab6()
{
	struct itimerval nval;
	
	signal(SIGALRM, sighandler);
	signal(SIGUSR1, sighandler);
	
	nval.it_interval.tv_sec = 1;
	nval.it_interval.tv_usec = 0;
	nval.it_value.tv_sec = 1;
	nval.it_value.tv_usec = 0;
	//raise(SIGINT);
	setitimer(ITIMER_REAL, &nval, 0);
	
	while (1) {
		pause();
	}
}

void sigalrm_proccess(int signo)
{
	int val;
	if (sc_regGet(IF, &val)) {
		return;
	}
	insp_reg++;
}

void sigusr1_proccess(int signo)
{
	rk_mytermrestor();
	sc_memoryInit();
	sc_regInit();
	if (sc_regSet(IF, 1)) {
		return;
	}
}

void print_interface()
{
	rk_mytermrestor();
	sc_memoryInit();
	sc_regInit();
	signal(SIGALRM, sigalrm_proccess);
	signal(SIGUSR1, sigusr1_proccess);

	struct itimerval nval;

	nval.it_interval.tv_sec = 1;
	nval.it_interval.tv_usec = 0;
	nval.it_value.tv_sec = 1;
	nval.it_value.tv_usec = 0;

	setitimer(ITIMER_REAL, &nval, 0);

	// set in memory multiplication table
	for (int i = 0; i < 10; i++) { 
		for (int j = 0; j < 10; j++) {
			sc_memorySet(i * 10 + j, i * j * pow(-1, i + j));
		}
	}
	int selected_x = 0, selected_y = 0;
	int exit_flag = 0;
	int tmp_val;
	enum keys key;
	mt_clrsrc();
	while (exit_flag == 0) {
		mi_printinterface(selected_x, selected_y);
		rk_readkey(&key);
		if (key == KEY_down && selected_y < 9) {
			selected_y++;
		} else if (key == KEY_up && selected_y > 0) {
			selected_y--;
		} else if (key == KEY_right && selected_x < 9) {
			selected_x++;
		} else if (key == KEY_left && selected_x > 0) {
			selected_x--;
		} else if ((key >= '0' && key <= '9') || (key >= 'a' && key <= 'f')) {
			if (sc_memoryGet(selected_y * 10 + selected_x, &tmp_val)) {
				printf("error memory address: %d cant read", selected_y * 10 + selected_x);
				return;
			}
			if (tmp_val >= 0) {
				tmp_val = (((key >= '0' && key <= '9') ? (key - '0') : (key - 'a' + 10)) | (tmp_val << 4)) & 0xFFFF;
			} else {
				tmp_val = 0 - ((((key >= '0' && key <= '9') ? (key - '0') : (key - 'a' + 10)) | ((0 - tmp_val) << 4)) & 0xFFFF);
			}
			if (sc_memorySet(selected_y * 10 + selected_x, tmp_val)) {
				printf("error memory address: %d cant writed", selected_y * 10 + selected_x);
				return;
			}
		} else if (key == KEY_backspace) {
			if (sc_memoryGet(selected_y * 10 + selected_x, &tmp_val)) {
				printf("error memory address: %d cant read", selected_y * 10 + selected_x);
				return;
			}
			if (tmp_val >= 0) {
				tmp_val = tmp_val >> 4 & 0xFFFF;
			} else {
				tmp_val = 0 - (((0 - tmp_val) & 0xFFFF) >> 4);
			}
			if (sc_memorySet(selected_y * 10 + selected_x, tmp_val)) {
				printf("error memory address: %d cant writed", selected_y * 10 + selected_x);
				return;
			}
		} else if (key == KEY_plus || key == KEY_minus) {
			if (sc_memoryGet(selected_y * 10 + selected_x, &tmp_val)) {
				printf("error memory address: %d cant read", selected_y * 10 + selected_x);
				return;
			}
			if ((tmp_val > 0 && key == KEY_minus) || (tmp_val < 0 && key == KEY_plus)) {
				tmp_val = 0 - tmp_val;
			}
			if (sc_memorySet(selected_y * 10 + selected_x, tmp_val)) {
				printf("error memory address: %d cant writed", selected_y * 10 + selected_x);
				return;
			}
		} else if (key == KEY_s) {
			sc_memorySave("default.bin");
			printf("save\n");
		} else if (key == KEY_l) {
			sc_memoryLoad("default.bin");
			printf("load\n");
		} else if (key == KEY_other || key == KEY_q) {
			exit_flag = 1;
		}
	}
}

int main()
{
	//print_test_lab1();
	//print_test_lab2();
	//print_test2_lab2();
	//print_test_lab4();
	//print_test_lab5();
	print_test_lab6();
	//print_interface();
	//mt_clrsrc();
	//for (int num = 0; num < 16; num++) {
		/*int tmp[2] = {0, 0};
		int tmp1 = 0;
		for (int i = 0; i < 2; i++) {
			for (int j = 0; j < 4; j++) {
				tmp1 = 0;
				for (int k = 0; k < 8; k++) {
					tmp1 |= ((big_char_spiral[i] >> (k + j * 8)) & 1) << (7 - k);
					//printf("%X\n%X\n", test[i], tmp1);
				}
				tmp[i] |= tmp1 << ((3 - j) * 8);
				//printf("%X\n%X\n", test[i], tmp[i]);
			}
			//printf("%X ", big_char_num[num][i], tmp[i]);
		}
		printf("{0x%08X, 0x%08X},\n", tmp[0], tmp[1]);*/
		//bc_printbigchar(tmp, 1, num * 9, 0, 1);
	//}
	return 0;
}
