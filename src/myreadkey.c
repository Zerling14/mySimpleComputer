#include <termios.h>
#include <unistd.h>
#include <stdio.h>
#include "myreadkey.h"
#include <string.h>

int rk_readkey(enum keys *key)
{
	struct termios options;
	char buff[20];
	int num_read;
	
	if (tcgetattr(STDIN_FILENO, &options) != 0) {
		return -1;
	}
	if(access("termsettings", F_OK) == -1) {
		rk_mytermsave();
	}
	if (rk_mytermregime(0, 0, 1, 0, 1) != 0) {
		return -1;
	}
	num_read = read(STDIN_FILENO, buff, 15);
	rk_mytermrestor();
	if (num_read < 0) {
		return -1;
	}
	buff[num_read] = 0;
	if ((buff[0] >= 'a' && buff[0] <= 'z') || buff[0] == '\n' || (buff[0] >= '0' && buff[0] <= '9') || buff[0] == 127 || buff[0] == 43 || buff[0] == 45) {
		*key = buff[0];
	} else if (strcmp(buff, "\E[15~") == 0) {
		*key = KEY_f5;
	} else if (strcmp(buff, "\E[17~") == 0) {
		*key = KEY_f6;
	} else if (strcmp(buff, "\E[A") == 0) {
		*key = KEY_up;
	} else if (strcmp(buff, "\E[B") == 0) {
		*key = KEY_down;
	} else if (strcmp(buff, "\E[C") == 0) {
		*key = KEY_right;
	} else if (strcmp(buff, "\E[D") == 0) {
		*key = KEY_left;
	} else {
		*key = KEY_other;
	}
	return 0;
}

int rk_mytermsave()
{
	struct termios options;
	FILE *save;
	
	if (tcgetattr(STDIN_FILENO, &options) != 0) {
		return -1;
	}
	if ((save = fopen("termsettings", "wb")) == NULL) {
		return -1;
	}
	fwrite(&options, sizeof(options), 1, save);
	fclose(save);
	
	return 0;
}

int rk_mytermrestor()
{
	struct termios options;
	FILE *save;
	
	if ((save = fopen("termsettings", "rb")) == NULL) {
		return -1;
	}
	if (fread(&options, sizeof(options), 1, save) > 0) {
		if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &options) != 0) {
			return -1;
		}
	} else {
		return -1;
	}
	return 0;
}

int rk_mytermregime(int regime, int vtime, int vmin, int echo, int sigint)
{
	struct termios options;
	
	if (tcgetattr(STDIN_FILENO, &options) != 0) {
		return -1;
	}
	if (regime == 1) {
		options.c_lflag |= ICANON;
	} else if (regime == 0) {
		options.c_lflag &= ~ICANON;
	} else {
		return -1;
	}
	if (regime == 0) {
		options.c_cc[VTIME] = vtime;
		options.c_cc[VMIN] = vmin;
		if (echo == 1) {
			options.c_lflag |= ECHO;
		} else if (echo == 0) {
			options.c_lflag &= ~ECHO;
		} else {
			return -1;
		}
		if (sigint == 1) {
			options.c_lflag |= ISIG;
		} else if (sigint == 0) {
			options.c_lflag &= ~ISIG;
		}
		else {
			return -1;
		}
	}
	if (tcsetattr(STDIN_FILENO, TCSANOW, &options) != 0) {
		return -1;
	}
	return 0;
}
