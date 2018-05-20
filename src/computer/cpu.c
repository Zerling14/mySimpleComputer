#include <stdio.h>
#include <string.h>
#include "interface.h"
#include "memory.h"
#include "cpu.h"
#include "command.h"

#define PRINT_COMMANDS_INTO_LOG

int ALU(int command, int operand)
{
	int value;
	char buff[20];
	#ifdef PRINT_COMMANDS_INTO_LOG
		int value2;
	#endif
	switch (command) {
	case COMMAND_ADD:
		#ifdef PRINT_COMMANDS_INTO_LOG
			if (sc_memoryGet(operand, &value2)) {
				return 1;
			}
			sprintf(buff, "ADD: %d[%X]\n", operand, value2);
			strcat(log_buff, buff);
			printf("%s", buff);
		#endif
		if (sc_memoryGet(operand, &value)) {
			return 1;
		}
		acc_reg = ((acc_reg & 0x7FFF) + (value & 0x7FFF)) & 0x7FFF;
		break;
	case COMMAND_SUB:
		#ifdef PRINT_COMMANDS_INTO_LOG
			if (sc_memoryGet(operand, &value2)) {
				return 1;
			}
			sprintf(buff, "SUB: %d[%X]\n", operand, value2);
			strcat(log_buff, buff);
			printf("%s", buff);
		#endif
		if (sc_memoryGet(operand, &value)) {
			return 1;
		}
		//acc_reg -= value;
		acc_reg = ((acc_reg & 0x7FFF) - (value & 0x7FFF)) & 0x7FFF;
		break;
	case COMMAND_DIVIDE:
		#ifdef PRINT_COMMANDS_INTO_LOG
			if (sc_memoryGet(operand, &value2)) {
				return 1;
			}
			sprintf(buff, "DIV: %d[%X]\n", operand, value2);
			strcat(log_buff, buff);
			printf("%s", buff);
		#endif
		if (sc_memoryGet(operand, &value)) {
			return 1;
		}
		acc_reg = acc_reg / value; //TODO
		break;
	case COMMAND_MUL:
		#ifdef PRINT_COMMANDS_INTO_LOG
			if (sc_memoryGet(operand, &value2)) {
				return 1;
			}
			sprintf(buff, "MUL: %d[%X]\n", operand, value2);
			strcat(log_buff, buff);
			printf("%s", buff);
		#endif
		if (sc_memoryGet(operand, &value)) {
			return 1;
		}
		if (!(acc_reg & 0x7FFF) || !(value & 0x7FFF)) {
			acc_reg = 0;
		} else {
			//acc_reg *= value;
			if (acc_reg & 0x4000) {
				if (value & 0x4000) {
					acc_reg = (((0 - acc_reg) & 0x3FFF) * ((0 - value) & 0x3FFF)) & 0x3FFF;
				} else {
					acc_reg = (((0 - acc_reg) & 0x3FFF) * (value & 0x3FFF)) & 0x3FFF;
					acc_reg ^= 0x4000;
				}	
			} else {
				if (value & 0x4000) {
					acc_reg = ((acc_reg & 0x3FFF) * ((0 - value) & 0x3FFF)) & 0x3FFF;
					acc_reg ^= 0x4000;
				} else {
					acc_reg = ((acc_reg & 0x3FFF) * (value & 0x3FFF)) & 0x3FFF;
				}	
			}
		}
		break;
	//	USERS COMMANDS
	case COMMAND_NOT:
		#ifdef PRINT_COMMANDS_INTO_LOG
			if (sc_memoryGet(operand, &value2)) {
				return 1;
			}
			sprintf(buff, "NOT: %d[%X]\n", operand, value2);
			strcat(log_buff, buff);
			printf("%s", buff);
		#endif
		if (sc_memorySet(operand, ~acc_reg)) {
			return 1;
		}
		break;
	case COMMAND_AND:
		#ifdef PRINT_COMMANDS_INTO_LOG
			if (sc_memoryGet(operand, &value2)) {
				return 1;
			}
			sprintf(buff, "AND: %d[%X]\n", operand, value2);
			strcat(log_buff, buff);
			printf("%s", buff);
		#endif
		if (sc_memoryGet(operand, &value)) {
			return 1;
		}
		acc_reg &= value;
		break;
	case COMMAND_OR:
		#ifdef PRINT_COMMANDS_INTO_LOG
			if (sc_memoryGet(operand, &value2)) {
				return 1;
			}
			sprintf(buff, "OR: %d[%X]\n", operand, value2);
			strcat(log_buff, buff);
			printf("%s", buff);
		#endif
		if (sc_memoryGet(operand, &value)) {
			return 1;
		}
		acc_reg |= value;
		break;
	case COMMAND_XOR:
		#ifdef PRINT_COMMANDS_INTO_LOG
			if (sc_memoryGet(operand, &value2)) {
				return 1;
			}
			sprintf(buff, "XOR: %d[%X]\n", operand, value2);
			strcat(log_buff, buff);
			printf("%s", buff);
		#endif
		if (sc_memoryGet(operand, &value)) {
			return 1;
		}
		acc_reg ^= value;
		break;
	}
	return 0;
}

int CU()
{
	int value;
	if (sc_memoryGet(insp_reg, &value)) {
		return 1;
	}
	int command;
	int operand;
	if (sc_commandDecode(value, &command, &operand)) {
		sc_regSet(IF, 1);
		return 1;
	}
	if ((command >= 30 && command <= 33) || (command >= 51 && command <= 54)) {
		if (ALU(command, operand)) {
			return 1;
		}
	} else {
		int value;
		char buff[20];
		#ifdef PRINT_COMMANDS_INTO_LOG
			int value2;
		#endif
		switch (command) {
		case COMMAND_READ: //READ
			#ifdef PRINT_COMMANDS_INTO_LOG
				sprintf(buff, "READ: %d\n", operand);
				strcat(log_buff, buff);
				printf("%s", buff);
			#endif
			scanf("%d", &value);
			if (sc_memorySet(operand, value)) {
				return 1;
			}
			break;
		case COMMAND_WRITE: //WRITE
			#ifdef PRINT_COMMANDS_INTO_LOG
				if (sc_memoryGet(operand, &value2)) {
					return 1;
				}
				sprintf(buff, "WRITE: %d[%d]\n", operand, value2);
				strcat(log_buff, buff);
				printf("%s", buff);
			#endif
			if (sc_memoryGet(operand, &value)) {
				return 1;
			}
			sprintf(buff, "%X\n", value);
			strcat(log_buff, buff);
			printf("%s", buff);
			break;
		case COMMAND_LOAD: //LOAD
			#ifdef PRINT_COMMANDS_INTO_LOG
				if (sc_memoryGet(operand, &value2)) {
					return 1;
				}
				sprintf(buff, "LOAD: %d[%X]\n", operand, value2);
				strcat(log_buff, buff);
				printf("%s", buff);
			#endif
			if (sc_memoryGet(operand, &value)) {
				return 1;
			}
			acc_reg = value;
			break;
		case COMMAND_STORE: //STORE
			#ifdef PRINT_COMMANDS_INTO_LOG
				if (sc_memoryGet(operand, &value2)) {
					return 1;
				}
				sprintf(buff, "STORE: %d[%X]\n", operand, value2);
				strcat(log_buff, buff);
				printf("%s", buff);
			#endif
			if (sc_memorySet(operand, acc_reg)) {
				return 1;
			}
			break;
		case COMMAND_SET:
			#ifdef PRINT_COMMANDS_INTO_LOG
				sprintf(buff, "SET: %d[%X]\n", operand, acc_reg);
				strcat(log_buff, buff);
				printf("%s", buff);
			#endif
			acc_reg = operand;
			break;
		case COMMAND_JUMP:
			#ifdef PRINT_COMMANDS_INTO_LOG
				sprintf(buff, "JUMP: %d[%X]\n", operand, insp_reg);
				strcat(log_buff, buff);
				printf("%s", buff);
			#endif
			insp_reg = operand;
			break;
		case COMMAND_JNEG:
			#ifdef PRINT_COMMANDS_INTO_LOG
				sprintf(buff, "JNEG: %d[%X]\n", operand, insp_reg);
				strcat(log_buff, buff);
				printf("%s", buff);
			#endif
			if (acc_reg & 0x4000) {
				insp_reg = operand;
			}
			break;
		case COMMAND_JZ:
			#ifdef PRINT_COMMANDS_INTO_LOG
				sprintf(buff, "JZ: %d[%X]\n", operand, insp_reg);
				strcat(log_buff, buff);
				printf("%s", buff);
			#endif
			if (acc_reg == 0) {
				insp_reg = operand;
			}
			break;
		case COMMAND_HALT: //HALT
			#ifdef PRINT_COMMANDS_INTO_LOG
				sprintf(buff, "HALT: %d\n", operand);
				strcat(log_buff, buff);
				printf("%s", buff);
			#endif
			sc_regSet(IF, 1);
			break;
		case COMMAND_NOP: //HALT
			#ifdef PRINT_COMMANDS_INTO_LOG
				sprintf(buff, "NOP: %d\n", operand);
				strcat(log_buff, buff);
				printf("%s", buff);
			#endif
			break;
		default:
			sc_regSet(IF, 1);
			#ifdef PRINT_COMMANDS_INTO_LOG
				sprintf(buff, "WRONG COMMAND:%d %d\n", command, operand);
				strcat(log_buff, buff);
				printf("%s", buff);
			#endif
			break;
		}
	}
	return 0;
}
