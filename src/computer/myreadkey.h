#ifndef MYREADKEY_H
#define MYREADKEY_H

enum keys {
	KEY_other = 0,
	KEY_f5,
	KEY_f6,
	KEY_up,
	KEY_down,
	KEY_left,
	KEY_right,
	KEY_enter,
	KEY_i = 105,
	KEY_l = 108,
	KEY_q = 113,
	KEY_r = 114,
	KEY_s = 115,
	KEY_t = 116,
	KEY_0 = 48,
	KEY_1 = 49,
	KEY_2 = 50,
	KEY_3 = 51,
	KEY_4 = 52,
	KEY_5 = 53,
	KEY_6 = 54,
	KEY_7 = 55,
	KEY_8 = 56,
	KEY_9 = 57,
	KEY_backspace = 127,
	KEY_plus = 43,
	KEY_minus = 45
};

int rk_readkey(enum keys *key);
int rk_mytermsave();
int rk_mytermrestor();
int rk_mytermregime(int regime, int vtime, int vmin, int echo, int sigint);

#endif
