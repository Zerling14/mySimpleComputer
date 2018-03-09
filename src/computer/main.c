#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/time.h>
#include <signal.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <string.h>
#include "memory.h"
#include "debug.h"
#include "myterm.h"
#include "interface.h"
#include "bigchars.h"
#include "myreadkey.h"
#include "cpu.h"
#include "command.h"

int kbhit(void)
{
	int			cnt = 0;
	int			error;
	static struct termios	Otty, Ntty;

	tcgetattr(0, &Otty);
	Ntty = Otty;

	Ntty.c_iflag		= 0;	   /* input mode	*/
	Ntty.c_oflag		= 0;	   /* output mode	*/
	Ntty.c_lflag		&= ~ICANON;/* raw mode 		*/
	Ntty.c_cc[VMIN]		= CMIN;	   /* minimum chars to wait for */
	Ntty.c_cc[VTIME]	= CTIME;   /* minimum wait time	*/

	if (0 == (error = tcsetattr(0, TCSANOW, &Ntty))) {
		struct timeval	tv;
		error     += ioctl(0, FIONREAD, &cnt);
		error     += tcsetattr(0, TCSANOW, &Otty);
		tv.tv_sec  = 0;
		tv.tv_usec = 100; /* insert a minimal delay */
		select(1, NULL, NULL, NULL, &tv);
	}
	rk_mytermrestor();
	return (error == 0 ? cnt : -1 );
}

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

int selected_x = 0, selected_y = 0;
void sigalrm_proccess(int signo)
{
	int val;
	if (sc_regGet(IF, &val)) {
		return;
	}
	if (!val) {
		int insp_reg_tmp = insp_reg;
		if (CU()) {
			sc_regSet(IF, 1);
		} else if (insp_reg_tmp == insp_reg) {
			insp_reg++;
		}
		mi_printinterface(selected_x, selected_y);
	}
	struct itimerval nval;
	nval.it_interval.tv_sec = 0;
	nval.it_interval.tv_usec = 0;
	nval.it_value.tv_sec = 0;
	nval.it_value.tv_usec = 100000;

	setitimer(ITIMER_REAL, &nval, 0);
}

void sigusr1_proccess(int signo)
{
	sc_memoryInit();
	sc_regInit();
	if (sc_regSet(IF, 1)) {
		return;
	}
}

void sigint_proccess(int signo)
{
	rk_mytermrestor();
	exit(0);
}

void sigwinch_proccess(int signo)
{
	mt_clrsrc();
	mi_printinterface(selected_x, selected_y);
}

void print_interface()
{
	rk_mytermrestor();
	sc_memoryInit();
	sc_regInit();
	sc_regSet(IF, 1);
	signal(SIGALRM, sigalrm_proccess);
	signal(SIGUSR1, sigusr1_proccess);
	signal(SIGINT, sigint_proccess);
	signal(SIGWINCH, sigwinch_proccess);
	struct itimerval nval;

	printf("\033[8;80;81t");

	nval.it_interval.tv_sec = 0;
	nval.it_interval.tv_usec = 0;
	nval.it_value.tv_sec = 1;
	nval.it_value.tv_usec = 0;

	setitimer(ITIMER_REAL, &nval, 0);

	// set in memory multiplication table
	if (0) {
		for (int i = 0; i < 10; i++) { 
			for (int j = 0; j < 10; j++) {
				sc_memorySet(i * 10 + j, i * j * pow(-1, i + j));
			}
		}
	} else {
		int value;
		sc_commandEncode(COMMAND_SET, 0, &value);
		sc_memorySet(0, value);

		sc_commandEncode(COMMAND_STORE, 97, &value);
		sc_memorySet(1, value);
		
		sc_commandEncode(COMMAND_SET, 1, &value);
		sc_memorySet(2, value);
		
		sc_commandEncode(COMMAND_STORE, 98, &value);
		sc_memorySet(3, value);
		
		sc_commandEncode(COMMAND_SET, 0, &value);
		sc_memorySet(4, value);
		
		sc_commandEncode(COMMAND_STORE, 99, &value);
		sc_memorySet(5, value);
		
		sc_commandEncode(COMMAND_READ, 96, &value);
		sc_memorySet(6, value);
		
		sc_commandEncode(COMMAND_LOAD, 96, &value);
		sc_memorySet(7, value);
		
		sc_commandEncode(COMMAND_SUB, 99, &value);
		sc_memorySet(8, value);
		
		sc_commandEncode(COMMAND_JZ, 21, &value);
		sc_memorySet(9, value);
		
		sc_commandEncode(COMMAND_LOAD, 97, &value);
		sc_memorySet(10, value);
		
		sc_commandEncode(COMMAND_STORE, 95, &value);
		sc_memorySet(11, value);
		
		sc_commandEncode(COMMAND_LOAD, 98, &value);
		sc_memorySet(12, value);
		
		sc_commandEncode(COMMAND_STORE, 97, &value);
		sc_memorySet(13, value);
		
		sc_commandEncode(COMMAND_LOAD, 95, &value);
		sc_memorySet(14, value);
		
		sc_commandEncode(COMMAND_ADD, 98, &value);
		sc_memorySet(15, value);
		
		sc_commandEncode(COMMAND_STORE, 98, &value);
		sc_memorySet(16, value);
		
		sc_commandEncode(COMMAND_SET, 1, &value);
		sc_memorySet(17, value);
		
		sc_commandEncode(COMMAND_ADD, 99, &value);
		sc_memorySet(18, value);
		
		sc_commandEncode(COMMAND_STORE, 99, &value);
		sc_memorySet(19, value);
		
		sc_commandEncode(COMMAND_JUMP, 7, &value);
		sc_memorySet(20, value);
		
		sc_commandEncode(COMMAND_WRITE, 97, &value);
		sc_memorySet(21, value);
		
		sc_commandEncode(COMMAND_HALT, 0, &value);
		sc_memorySet(22, value);
		/*
	0	SET 0
	1	STORE 97 //a
	2	SET 1
	3	STORE 98 //b
	4	SET 0
	5	STORE 99 //i
	6	READ 96  //num
	7	LOAD 96
	8	SUB 99   //num - i
	9	JZ write://num - i == 0
	10	LOAD 97
	11	STORE 95 //tmp = a
	12	LOAD 98
	13	STORE 97 //a = b
	14	LOAD 95
	15	ADD 98
	16	STORE 98 //b = tmp + b
	17	SET 1
	18	ADD 99
	19	STORE 99
	20	JUMP 9
	21	write: WRITE 97
	22	HALT
		*/
	}
	int exit_flag = 0;
	int tmp_val;
	enum keys key;
	mt_clrsrc();
	
	mi_printinterface(selected_x, selected_y);
	while (exit_flag == 0) {
		int ignore_val;
		if (sc_regGet(IF, &ignore_val)) {
			return;
		}
		if (ignore_val) {
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
			} else if (key == KEY_r) {
				sc_regSet(IF, 0);
				strcat(log_buff, "run\n");
			} else if (key == KEY_s) {
				strcat(log_buff, "save file name: ");
				printf("save file name: ");
				char buff[20];
				scanf("%s", buff);
				strcat(log_buff, buff);
				strcat(log_buff, "\n");
				sc_memorySave(buff);
			} else if (key == KEY_l) {
				strcat(log_buff, "load file name: ");
				printf("load file name: ");
				char buff[20];
				scanf("%s", buff);
				strcat(log_buff, buff);
				strcat(log_buff, "\n");
				sc_memoryLoad(buff);
			} else if (key == KEY_i) {
				raise (SIGUSR1);
				strcat(log_buff, "reset\n");
			} else if (key == KEY_t) {
				int insp_reg_tmp = insp_reg;
				if (CU()) {
					sc_regSet(IF, 1);
				} else if (insp_reg_tmp == insp_reg) {
					insp_reg++;
				}
				strcat(log_buff, "step\n");
			} else if (key == KEY_f5) {
				strcat(log_buff, "accumulator value: ");
				printf("accumulator value: ");
				int buff;
				scanf("%X", &buff);
				char buff_char[10];
				sprintf(buff_char, "%X\n", buff);
				strcat(log_buff, buff_char);
				acc_reg = buff;
				mi_printinterface(selected_x, selected_y);
			} else if (key == KEY_f6) {
				strcat(log_buff, "instructionCounter value: ");
				printf("instructionCounter value: ");
				int buff;
				scanf("%X", &buff);
				char buff_char[10];
				sprintf(buff_char, "%X\n", buff);
				strcat(log_buff, buff_char);
				insp_reg = buff;
				mi_printinterface(selected_x, selected_y);
			} else if (key == KEY_other || key == KEY_q) {
				exit_flag = 1;
			}
			mi_printinterface(selected_x, selected_y);
		}
	}
	rk_mytermrestor();
}

int main()
{
	//print_test_lab1();
	//print_test_lab2();
	//print_test2_lab2();
	//print_test_lab4();
	//print_test_lab5();
	//print_test_lab6();
	print_interface();
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
