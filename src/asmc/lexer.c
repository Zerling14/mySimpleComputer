#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "printinfo.h"
#include "lexer.h"
#include "command.h"
#include "myterm.h"
#include "memory.h"

int cstrtocint(char *cstr, int *cint)
{
	if (!strcmp(cstr, "READ")) {
		*cint = COMMAND_READ;
	} else if (!strcmp(cstr, "WRITE")) {
		*cint = COMMAND_WRITE;
	} else if (!strcmp(cstr, "LOAD")) {
		*cint = COMMAND_LOAD;
	} else if (!strcmp(cstr, "STORE")) {
		*cint = COMMAND_STORE;
	} else if (!strcmp(cstr, "SET")) {
		*cint = COMMAND_SET;
	} else if (!strcmp(cstr, "ADD")) {
		*cint = COMMAND_ADD;
	} else if (!strcmp(cstr, "SUB")) {
		*cint = COMMAND_SUB;
	} else if (!strcmp(cstr, "DIVIDE")) {
		*cint = COMMAND_DIVIDE;
	} else if (!strcmp(cstr, "MUL")) {
		*cint = COMMAND_MUL;
	} else if (!strcmp(cstr, "JUMP")) {
		*cint = COMMAND_JUMP;
	} else if (!strcmp(cstr, "JNEG")) {
		*cint = COMMAND_JNEG;
	} else if (!strcmp(cstr, "JZ")) {
		*cint = COMMAND_JZ;
	} else if (!strcmp(cstr, "HALT")) {
		*cint = COMMAND_HALT;
	} else if (!strcmp(cstr, "NOT")) {
		*cint = COMMAND_NOT;
	} else if (!strcmp(cstr, "AND")) {
		*cint = COMMAND_AND;
	} else if (!strcmp(cstr, "OR")) {
		*cint = COMMAND_OR;
	} else if (!strcmp(cstr, "XOR")) {
		*cint = COMMAND_XOR;
	} else if (!strcmp(cstr, "NOP")) {
		*cint = COMMAND_NOP;
	} else {
		return -1;
	}
	return 0;
}

int calcspaces(char *code_token, int num)
{
	if (num < 0 || !code_token) {
		return -1;
	}
	int num_spaces = 0;
	while (num > 0) {
		if (*code_token == '\t') {
			num_spaces += 8 - (num_spaces % 8);
		} else {
			num_spaces++;
		}
		code_token++;
		num--;
	}
	return num_spaces;
}

int tonumber(char *s, int *number)
{
	if (s == NULL || *s == '\0' || isspace(*s)) {
		return 0;
	}
	char *p;
	*number = (int)strtod(s, &p);
	return *p == '\0';
}

int lexer(char *infile, char *outfile)
{
	int indesc = open(infile, O_RDONLY);
	
	if (indesc == -1) {
		print_error("error", "cant open infile");
	}

	off_t size_file = lseek(indesc, 0, SEEK_END);
	if (size_file == -1L) {
		print_error("error", "cant read infile size");
		return -1;
	}
	if (lseek(indesc, 0, SEEK_SET) == -1L) {
		print_error("error", "cant read infile size");
		return -1;
	}
	//printf("find %ld bytes\n", size_file);
	
	char *code_buff = calloc(size_file, 1);
	
	if (size_file != read(indesc, code_buff, size_file)) {
		print_error("error", "cant read infile fully");
		return -1;
	}
	
	if (close(indesc) == -1) {
		print_error("error", "cant close infile");
	}
	
	int memory_buff[100] = {};
	char code_line_buff[100];
	int num_line = 0;
	
	int error_flag = 0;
	
	char *saveptr1;
	char *code_line = strtok_r(code_buff, "\n", &saveptr1);
	while (code_line) {
		strncpy(code_line_buff, code_line, 99);
		code_line_buff[99] = 0;
		//printf("%s\n", code_line);
		
		int num_memory_cell = -1, command = 0, operand = 0, num_token = 0;
		
		char *saveptr2;
		char *code_token = strtok_r(code_line, " \t\n", &saveptr2);
		while (code_token) {
			int num_spaces = calcspaces(code_line_buff, code_token - code_line);
			//printf("%d\n", num_spaces);
			
			if (num_token == 0) {
				int number;
				if (!tonumber(code_token, &number) || number < 0 || number > 99) {
					char buff[200];
					sprintf(buff, "cell address \"%s\" not exist\n%s\n\E[32m\E[1m^\E[0m", code_token, code_line_buff);
					print_error("error", buff);
					error_flag = 1;
				}
				num_memory_cell = number;
			} else if (num_token == 1) {
				if (cstrtocint(code_token, &command) == -1) {
					char buff[200];
					sprintf(buff, "unknown command \"%s\" line %d\n%s\n\E[32m\E[1m%*c\E[0m", code_token, num_line, code_line_buff, num_spaces + 1, '^');
					print_error("error", buff);
					error_flag = 1;
				}
			} else if (num_token == 2) {
				int number;
				if (!tonumber(code_token, &number) || number < 0 || number > 127) {
					char buff[200];
					sprintf(buff, "invalid operand \"%s\" not exist\n%s\n\E[32m\E[1m%*c\E[0m", code_token, code_line_buff, num_spaces + 1, '^');
					print_error("error", buff);
					error_flag = 1;
				}
				operand = number;
			} else if (num_token == 3) {
				if (code_token[0] != ';') {
					char buff[200];
					sprintf(buff, "unknown token \"%s\" line %d use \";\" for commentary\n%s\n\E[32m\E[1m%*c\E[0m", code_token, num_line, code_line_buff, num_spaces + 1, '^');
					print_error("error", buff);
					error_flag = 1;
				}
				break;
			} else {
				break;
			}
			code_token = strtok_r(0, " \t\n", &saveptr2);
			num_token++;
		}
		//printf("%d %d %d\n", num_memory_cell, command, operand);
		
		int coded_command;
		sc_commandEncode(command, operand, &coded_command);
		
		memory_buff[num_memory_cell] = coded_command;
		
		//printf("%X\n", coded_command);
		
		
		
		code_line = strtok_r(0, "\n", &saveptr1);
		num_line++;
		//printf("\n");
	}
	if (error_flag) {
		return -1;
	}

	int outdesc = open(outfile, O_WRONLY | O_TRUNC | O_CREAT, -1);
	
	if (outdesc == -1) {
		print_error("error", "cant open outfile");
	}
	
	if (write(outdesc, memory_buff, 400) < 0) {
		print_error("error", "cant write in outfine");
		return -1;
	}
	
	if (close(outdesc) == -1) {
		print_error("error", "cant close outfile");
	}
	return 0;
}
