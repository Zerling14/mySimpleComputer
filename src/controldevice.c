#include <stdio.h>
#include <stdlib.h>
#include "controldevice.h"

int sc_memoryInit()
{
	for (int i = 0; i < MEMORY_SIZE; i++) {
		memory[i] = 0;
	}

	return 0;
}

int sc_memorySet(int address, int value)
{
	if (address < 0 || address > 99) {
		sc_regSet (MF, 1);
		return 1;
	}
	memory[address] = value;

	return 0;
}

int sc_memoryGet(int address, int *value)
{
	if (address < 0 || address > 99 || !value) {
		return 1;
	}

	*value = memory[address];

	return 0;
}

int sc_memorySave(char *filename)
{
	if (!filename) {
		return 1;
	}

	FILE *ptrFile = fopen(filename, "wb");
	if (!ptrFile) {
		return 1;
	}

	fwrite(memory, CELL_SIZE, MEMORY_SIZE, ptrFile);
	fclose(ptrFile);
	return 0;
}

int sc_memoryLoad(char* filename)
{
	if (!filename) {
		return 1;
	}

	FILE *ptrFile = fopen(filename, "rb");
	if (!ptrFile) {
		return 1;
	}
	fread(memory, CELL_SIZE, MEMORY_SIZE, ptrFile);

	fclose(ptrFile);

	return 0;
}

int sc_regInit()
{
	flag_reg = 0;
	return 0;
}

int sc_regSet(int num_register, int value)
{
	if (num_register < 0 || num_register > 4) {
		return 1;
	}

	if (value == 1) {
		flag_reg |= (1 << num_register);
	} else if (value == 0) {
		flag_reg &= ~(1 << num_register);
	} else {
		return 1;
	}

	return 0;
}

int sc_regGet(int num_register, int *value)
{
	if (num_register < 0 || num_register > 4 || !value) {
		return 1;
	}
	if (flag_reg & 1 << num_register) {
		*value = 1;
	} else {
		*value = 0;
	}

	return 0;
}

int processor_commands[] = {10, 11, 20, 21, 30, 31, 32, 33, 40, 41, 42, 43};
int user_commands_start = 51;
int user_commands_end = 76;

int sc_commandEncode(int command, int operand, int *value)
{
	if (!value) {
		return 1;
	}
	int valid_command = 0;
	int count_processor_commands = sizeof(processor_commands)/sizeof(processor_commands[0]);
	for (int i = 0; i < count_processor_commands; i++) {
		if (command == processor_commands[i]) {
			valid_command = 1;
			break;
		}
	}
	if (command >= user_commands_start && command <= user_commands_end) {
		valid_command = 1;
	}
	if (!valid_command) {
		return 1;
	}
	if (operand < 0 || operand > 127) {
		return 1;
	}
	*value = operand | command << 7;
	return 0;
}
int sc_commandDecode(int value, int *command, int *operand)
{
	if (!command || !operand) {
		return 1;
	}
	if (value & 0x4000) {
		sc_regSet(EF, 1);
		return 1;
	}
	*command = (value & 0x3F00) >> 7;
	*operand = value & 0x7F;
	return 0;
}

