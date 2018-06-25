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
		mi_printinterface(insp_reg % 10, insp_reg / 10);
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

char *need_load = 0;
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

	//printf("\033[8;80;81t"); // try set console size, actualy bad idea :(

	nval.it_interval.tv_sec = 0;
	nval.it_interval.tv_usec = 0;
	nval.it_value.tv_sec = 1;
	nval.it_value.tv_usec = 0;

	setitimer(ITIMER_REAL, &nval, 0); // init timer for incrementing instruction counter

	if (need_load) {
		sc_memoryLoad(need_load);
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
					tmp_val = (((key >= '0' && key <= '9') ? (key - '0') : (key - 'a' + 10)) | (tmp_val << 4));
				} else {
					tmp_val = 0 - ((((key >= '0' && key <= '9') ? (key - '0') : (key - 'a' + 10)) | ((0 - tmp_val) << 4)));
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
				if (key == KEY_plus) {
					tmp_val = tmp_val & (~0x4000);
				}
				if (key == KEY_minus) {
					tmp_val = tmp_val | 0x4000;
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
				selected_x = insp_reg % 10;
				selected_y = insp_reg / 10;
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

int main(int argc, char* argv[])
{
	if (argc == 2) {
		need_load = argv[1];
	}

	print_interface();

	return 0;
}
