#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "basicprintinfo.h"
#include "basiclexer.h"
#include "command.h"
#include "myterm.h"
#include "memory.h"

/* typedef struct {
	int   basic_line_num;
	char *basic_str;
	char *basic_command;
	char *basic_operand;
	int   translated_line_num;
	char *translated_str;
} LineStruct; */

int basic_last_num_line = 0;
int translate_basic(LineStruct *line_struct)
{
	if (!line_struct->basic_str) {
		print_error("internal error", "couldnt find basic_str");
		return -1;
	}
	
	char code_line_buff[100];
	strncpy(code_line_buff, line_struct->basic_str, 99);
	code_line_buff[99] = 0;
	
	int error_flag = 0;
	
	int num_token = 0;
	
	char *saveptr;
	char *code_token = strtok_r(line_struct->basic_str, " \t\n", &saveptr);
	while (code_token) {
		int num_spaces = calcspaces(code_line_buff, code_token - line_struct->basic_str);

		//printf("%d\n", num_spaces);
		
		if (num_token == 0) {
			int number;
			if (!tonumber(code_token, &number) || (number - basic_last_num_line != 10)) {
				char buff[200];
				sprintf(buff, "line number \"%s\" is wrong\n%s\n\E[32m\E[1m^\E[0m", code_token, code_line_buff);
				print_error("error", buff);
				error_flag = 1;
			}
			basic_last_num_line = number;
			line_struct->basic_line_num = number;
		} else if (num_token == 1) {
			if (validate_command(code_token) == -1) {
				char buff[200];
				sprintf(buff, "unknown command \"%s\" line %d\n%s\n\E[32m\E[1m%*c\E[0m", code_token, line_struct->basic_line_num, code_line_buff, num_spaces + 1, '^');
				print_error("error", buff);
				error_flag = 1;
			}
			line_struct->basic_command = code_token;
			break;
		} else if (num_token >= 2) {
			if (validate_operand(code_token) == -1) {
				char buff[200];
				sprintf(buff, "invalid operand \"%s\" not exist\n%s\n\E[32m\E[1m%*c\E[0m", code_token, code_line_buff, num_spaces + 1, '^');
				print_error("error", buff);
				error_flag = 1;
			}
			//strcat(code_token, saveptr);
			line_struct->basic_operand = code_token;
			break;
		} else {
			error_flag = 1;
			break;
		}
		code_token = strtok_r(0, " \t\n", &saveptr);
		num_token++;
	}
	line_struct->basic_operand = saveptr;
	
	if (error_flag) {
		return -1;
	}
	printf("1)%d 2)%s 3)%s\n", line_struct->basic_line_num, line_struct->basic_command, line_struct->basic_operand);
	
	//analysis basic command
	char *cstr = line_struct->basic_operand;
	if (!strcmp(cstr, "REM")) {
		line_struct->translated_str = calloc(1, sizeof(char));
	} else if (!strcmp(cstr, "INPUT")) {
		char *translated_str_buff[1000];
		get_var_address()
		//calculate_expression(translated_str_buff, cstr);
	} else if (!strcmp(cstr, "OUTPUT")) {
		char *translated_str_buff[1000];
		calculate_expression(translated_str_buff, cstr);
	} else if (!strcmp(cstr, "GOTO")) {
	} else if (!strcmp(cstr, "IF")) {
	} else if (!strcmp(cstr, "LET")) {
	} else if (!strcmp(cstr, "END")) {
	}
	
	return 0;
}

int validate_operand(char *cstr)
{
	return 0;
}

//REM, INPUT, OUTPUT, GOTO, IF, LET, END.
int validate_command(char *cstr)
{
	if (!strcmp(cstr, "REM")) {
	} else if (!strcmp(cstr, "INPUT")) {
	} else if (!strcmp(cstr, "OUTPUT")) {
	} else if (!strcmp(cstr, "GOTO")) {
	} else if (!strcmp(cstr, "IF")) {
	} else if (!strcmp(cstr, "LET")) {
	} else if (!strcmp(cstr, "END")) {
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
	
	for (int i = 0; i < strlen(code_buff); ++i) {
		if (code_buff[i] == '\r') {
			code_buff[i] = ' ';
		}
	}
	
	int memory_buff[100] = {};
	char code_line_buff[100];
	int num_line = 0;
	
	int error_flag = 0;
	
	char *saveptr1;
	char *code_line = strtok_r(code_buff, "\n", &saveptr1);
	
	LineStruct *line_struct_arr = calloc(20, sizeof(LineStruct));
	int line_struct_arr_size = 20;
	int line_struct_arr_size_exp = 20;
	int line_struct_arr_size_now = 0;
	
	while (code_line) {
		strncpy(code_line_buff, code_line, 99);
		code_line_buff[99] = 0;
		
		//fill struct
		if (line_struct_arr_size_now >= line_struct_arr_size) { // expanse vector
			line_struct_arr_size += line_struct_arr_size_exp;
			line_struct_arr = realloc(line_struct_arr, line_struct_arr_size * sizeof(LineStruct));
			memset(line_struct_arr + line_struct_arr_size_now, 0, line_struct_arr_size_exp);
		}
		
		// copy string for future analysis
		char *basic_str_buff = calloc(strlen(code_line_buff), sizeof(char));
		strcpy(basic_str_buff, code_line_buff);
		line_struct_arr[line_struct_arr_size_now].basic_str = basic_str_buff;
		
		
		// analis and fill struct
		if (translate_basic(&line_struct_arr[line_struct_arr_size_now])) {
			error_flag = 1;
		}
		
		++line_struct_arr_size_now;
		code_line = strtok_r(0, "\n", &saveptr1);
		num_line++;
		//printf("\n");
	}
	if (error_flag) {
		return -1;
	}
	
	for (int i = 0; i < line_struct_arr_size_now; ++i) {
		//printf("%s\n", line_struct_arr[i].basic_str);
		free(line_struct_arr[i].basic_str);
	}

	free(line_struct_arr);
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
