#include <stdio.h>
#include <string.h>
#include "interface.h"
#include "memory.h"
#include "cpu.h"

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
		acc_reg += value;
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
		acc_reg -= value;
		break;
	case COMMAND_DIVIDE:
		break;
	case COMMAND_MUL:
		break;
	//	USERS COMMANDS
	case COMMAND_NOT:
		break;
	case COMMAND_AND:
		break;
	case COMMAND_OR:
		break;
	case COMMAND_XOR:
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
				sprintf(buff, "LOAD: %d[%X]\n", operand, value2);
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
			insp_reg = operand;
			break;
		case COMMAND_JNEG:
			if (acc_reg < 0) {
				insp_reg = operand;
			}
			break;
		case COMMAND_JZ:
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
