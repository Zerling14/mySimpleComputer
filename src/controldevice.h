#ifndef CONTOLDEVICE_H
#define CONTOLDEVICE_H

#define MEMORY_SIZE 100
#define CELL_SIZE sizeof(int)

//FLAGS 
// bit number in flags register
#define CF 0	// Carry Flag
#define DF 1	// Divine on Zero Flag
#define MF 2	// Memory Out Flag
#define IF 3	// Ignore Tack Impulse Flag
#define EF 4	// Error Execute Command Flag

int memory[MEMORY_SIZE];
int flag_reg;
int insp_reg;
int acc_reg;

int sc_memoryInit();
int sc_memorySet(int address, int value);
int sc_memoryGet(int address, int *value);
int sc_memorySave(char *filename);
int sc_memoryLoad(char* filename);
int sc_regInit();
int sc_regSet(int num_register, int value);
int sc_regGet(int num_register, int *value);

int sc_commandEncode(int command, int operand, int *value);
int sc_commandDecode(int value, int *command, int *operand);

#endif
