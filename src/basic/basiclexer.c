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
#include "datalist.c"

int basic_last_num_line = 0;
int asm_last_memory_cell = 0;

int exp_delim_size = 9;
char *expression_delimiters[9] = {"*", "/", "+", "-", "(", ")", "<", ">", "="};
int line_delim_size = 7;
char *line_delimiters[7] = {"GOTO", "END", "IF", "LET", "OUTPUT", "INPUT", "REM"};

DataList *line_struct_list;

char* strtok_r(char *str, const char *delim, char **nextp)
{
	char *ret;
	if (str == NULL) {
		str = *nextp;
	}
	str += strspn(str, delim);
	if (*str == '\0') {
		return NULL;
	}
	ret = str;
	str += strcspn(str, delim);
    if (*str) {
		*str++ = '\0';
	}
	*nextp = str;
	return ret;
}

AsmCommandList *AsmCommandPushEx(AsmCommandList *list, int command, int operand, int num)
{
	AsmCommandList *tmp = list;
	while(tmp->next != 0) {
		tmp = tmp->next;
	}
	if (tmp->init) {
		tmp->next = calloc(1, sizeof(AsmCommandList));
		tmp = tmp->next;
	}
	tmp->init = 1;
	tmp->current.command = command;
	tmp->current.operand = operand;
	tmp->current.num = num;
	return tmp;
}

AsmCommandList *AsmCommandPush(AsmCommandList *list, int command, int operand)
{
	return AsmCommandPushEx(list, command, operand, asm_last_memory_cell++);
}



void AsmCommandCat(AsmCommandList *list1, AsmCommandList *list2)
{
	AsmCommandList *tmp = list1;
	if (!list1 || !list2 || !list2->init) {
		return;
	}
	while(tmp->next) {
		tmp = tmp->next;
	}
	if (tmp->init) {
		tmp->next = list2;
	} else {
		tmp->init = 1;
		tmp->current = list2->current;
		tmp->next = list2->next;
	}
}

void AsmCommandPrintEx(AsmCommandList *list, int out)
{
	//AsmCommandPrintEx(list, 1);
	AsmCommandList *node = list;
	
	char buff[256];
	while (node) {
		if (node->init) {
			sprintf(buff, "%d ", node->current.num);
			write(out, buff, strlen(buff));
			
			switch (node->current.command) {
			case COMMAND_READ:
				sprintf(buff, "READ");
				write(out, buff, strlen(buff));
				break;
			case COMMAND_WRITE:
				sprintf(buff, "WRITE");
				write(out, buff, strlen(buff));
				break;
			case COMMAND_LOAD:
				sprintf(buff, "LOAD");
				write(out, buff, strlen(buff));
				break;
			case COMMAND_STORE:
				sprintf(buff, "STORE");
				write(out, buff, strlen(buff));
				break;
			case COMMAND_ADD:
				sprintf(buff, "ADD");
				write(out, buff, strlen(buff));
				break;
			case COMMAND_SUB:
				sprintf(buff, "SUB");
				write(out, buff, strlen(buff));
				break;
			case COMMAND_DIVIDE:
				sprintf(buff, "DIVIDE");
				write(out, buff, strlen(buff));
				break;
			case COMMAND_MUL:
				sprintf(buff, "MUL");
				write(out, buff, strlen(buff));
				break;
			case COMMAND_JUMP:
				sprintf(buff, "JUMP");
				write(out, buff, strlen(buff));
				break;
			case COMMAND_JNEG:
				sprintf(buff, "JNEG");
				write(out, buff, strlen(buff));
				break;
			case COMMAND_JZ:
				sprintf(buff, "JZ");
				write(out, buff, strlen(buff));
				break;
			case COMMAND_HALT:
				sprintf(buff, "HALT");
				write(out, buff, strlen(buff));
				break;
			case COMMAND_EQUAL:
				sprintf(buff, "=");
				write(out, buff, strlen(buff));
			}
			sprintf(buff, " %d\n", node->current.operand);
			write(out, buff, strlen(buff));
		}
		node = node->next;
	}
}

void AsmCommandPrint(AsmCommandList *list)
{
	AsmCommandPrintEx(list, 1);
}

int getPriority(char op)
{
    switch (op)
    {
        case '(':
            return -1; // не выталкивает сам и не дает вытолкнуть себя другим
		case '*': case '/':
            return 1;
        case '+': case '-':
            return 2;
		case '<': case '>': case '=':
			return 3;
        default:
			return 0;
    }
}

int canPop(char op1, DataList *Functions)
{
    if (datalist_size(Functions) == 0)
        return 0;
    int p1 = getPriority(op1);
    int p2 = getPriority(*(char*)datalist_peek(Functions));
	if (p1 == 0 || p2 == 0) {
		printf("error %c\n", op1);
	}
    return p1 >= 0 && p2 >= 0 && p1 >= p2;
}	

#define OPERATION_LESS 1
#define OPERATION_MORE 2
#define OPERATION_EQUAL 3

int tmp_var_num = 0;
char last_tmp_var_in_acc = -1;
int last_tmp_var_in_acc_num = -1;
int last_operation = -1;
AsmCommandList *popFunction(DataList *Operands, DataList *Functions)
{
	AsmCommandList *list = calloc(1, sizeof(AsmCommandList));
    char *A = datalist_pop_back(Operands);
    char *B = datalist_pop_back(Operands);
    //char buff[256] = "";
	//printf("%s %s\n", A, B);
	
	if (last_tmp_var_in_acc != *B) {
		if (last_tmp_var_in_acc != -1) {
			int var_add = get_tmp_var_address(last_tmp_var_in_acc);
			AsmCommandPush(list, COMMAND_STORE, var_add);
		}
		if (isalpha(*B)) {
			int var_cell;
			if (isupper(*B)) {
				var_cell = get_var_address(B);
			} else {
				var_cell = get_tmp_var_address(*B);
				//printf("%d %s\n", var_cell, B);
			}
			AsmCommandPush(list, COMMAND_LOAD, var_cell);
		} else {
			int var_cell = get_num_var_address(atoi(B));
			AsmCommandPush(list, COMMAND_LOAD, var_cell);
		}
	}
	int var_cell = -1;
	
	if (isalpha(*A)) {
		if (isupper(*A)) {
			var_cell = get_var_address(A);
		} else {
			var_cell = get_tmp_var_address(*A);
			//printf("%d %s\n", var_cell, A);
		}
	} else {
		var_cell = get_num_var_address(atoi(A));
		printf("%s %d\n", A, var_cell);
	}
	char operation = *(char*)datalist_pop_back(Functions);
    switch (operation)
    {
        case '+':
			if (var_cell != -1) {
				AsmCommandPush(list, COMMAND_ADD, var_cell);
			}
            break;
        case '-': //int_vector_push_back(Operands, A - B);
			if (var_cell != -1) {
				AsmCommandPush(list, COMMAND_SUB, var_cell);
			}
            break;
        case '*': //int_vector_push_back(Operands, A * B);
			if (var_cell != -1) {
				AsmCommandPush(list, COMMAND_MUL, var_cell);
			}
            break;
        case '/': //int_vector_push_back(Operands, A / B);
			if (var_cell != -1) {
				AsmCommandPush(list, COMMAND_DIVIDE, var_cell);
			}
            break;
		case '<':
			if (var_cell != -1) {
				AsmCommandPush(list, COMMAND_SUB, var_cell);
				AsmCommandPush(list, COMMAND_MUL, get_num_var_address(-1));
				
				last_operation = OPERATION_LESS;
			}
            break;
		case '>':
			if (var_cell != -1) {
				AsmCommandPush(list, COMMAND_SUB, var_cell);
				last_operation = OPERATION_MORE;
			}
            break;
		case '=':
			if (var_cell != -1) {
				AsmCommandPush(list, COMMAND_SUB, var_cell);
				last_operation = OPERATION_EQUAL;
			}
            break;
		default:
			printf("che eto za simvol????? :%c\n", operation);
    }
	char *arr = calloc(2, sizeof(char));
	arr[0] = 'a' + tmp_var_num++;
	last_tmp_var_in_acc = arr[0];
	datalist_push_back(Operands, arr);
	
    return list;
}

AsmCommandList *translate_expression(char *cstr)
{
	//char translated_code[256] = "";
	tmp_var_num = 0;
	last_tmp_var_in_acc = -1;
	last_tmp_var_in_acc_num = -1;
	last_operation = -1;
	
	
	char *expression = calloc(strlen(cstr) + 6, sizeof(char));
	sprintf(expression, "( %s ) ", cstr);
	char *expression_str = calloc(strlen(expression) + 1, sizeof(char));
	char *expression_addres = expression_str;
	strcpy(expression_str, expression);
	DataList *Operands = datalist_new(10);
	DataList *Functions = datalist_new(10);
	AsmCommandList *list = calloc(1, sizeof(AsmCommandList));
	//char *prev_token = "";
	char *saveptr;
	char *code_token = strtok_r(expression_str, " ", &saveptr);
	while (code_token) {
		//printf("che: %s\n", code_token);
		if (isdigit(*code_token)) {
			datalist_push_back(Operands, code_token);
		} else if (isalpha(*code_token)) {
			if (!isupper(*code_token)) {
				return 0;
			}
			datalist_push_back(Operands, code_token);
		} else {
			int flag = 0;
            for (int j = 0; j < exp_delim_size; j++) {
            	if (!strcmp(code_token, expression_delimiters[j])) {
            		flag = 1;
            	}
            }
            if (flag) {
            	if (!strcmp(code_token, ")")) {
            		while (datalist_size(Functions) > 0 && strcmp(datalist_peek(Functions), "(")) {
                    	//AsmCommandList asm_code = popFunction(Operands, Functions);
						AsmCommandCat(list, popFunction(Operands, Functions));
                    	//if (asm_code) {
                    	//	//strcat(translated_code[256], asm_code);
                    	//}
                    }
               	 	datalist_pop_back(Functions);
            	} else {
            		while (canPop(*code_token, Functions)) { // Если можно вытолкнуть
                    	AsmCommandCat(list, popFunction(Operands, Functions)); // то выталкиваем
                    }
               		datalist_push_back(Functions, code_token); // Кидаем новую операцию в стек
            	}
            } else {
            	return 0;
            }
		}
		//prev_token = code_token;
		//printf("%s\n", code_token);
		code_token = strtok_r(NULL, " ", &saveptr);
	}
	
	char *tmp = datalist_pop_back(Operands);
	//printf("che: %s\n", tmp);
	if (last_tmp_var_in_acc != *tmp) {
		int var_cell = get_var_address(tmp);
		if (isalpha(*tmp)) {
			if (isupper(*tmp)) {
				var_cell = get_var_address(tmp);
			} else {
				var_cell = get_tmp_var_address(*tmp);
				//printf("%d %s\n", var_cell, B);
			}
		} else {
			var_cell = get_num_var_address(atoi(tmp));
		}
		AsmCommandPush(list, COMMAND_LOAD, var_cell);
	}
	/*for (int i = 0; i < strlen(token); i++) {
		if (isblank(token[i])) {
			continue;
		} else if (isalpha(token[i])) {
			if (!isupper(token[i])) {
				return 0;
			}
		} else if (isdigit(token[i])) {
			//int_vector_push_back(IntVector *v, int item)
		} else {
			
		}
	}*/
	free(expression_addres);
	return list;
}

// Get tokens separated by delimiters strings
char *get_token(char **str_p, char *delimiters[], int size)
{
    char *str = *str_p;
    char buff[256] = {0};
    int buff_index = 0;
    int start_token = 0;
    for (int i = 0; i <= strlen(str) + 1; ++i) {
        if (str[i] == 0 && i == 0) {
            return 0;
        }
        if (str[i] == ' ' || str[i] == 0) {
            int flag = 0;
            for (int j = 0; j < size; j++) {
            	if (!strcmp(buff, delimiters[j])) {
            		flag = 1;
            	}
            }
            if (flag) {
                if (start_token != 0) {
                    str[start_token] = 0;
                    *str_p = str + start_token + 1;
                    return str;
                }
                str[i] = 0;
                *str_p = str + i + 1;
                return str;
            } else {
                if (str[i] == 0) {
                    *str_p = str + i;
                    return str;
                }
            }
            start_token = i;
            buff[0] = 0;
            buff_index = 0;
            continue;
        }
        buff[buff_index] = str[i];
        buff[buff_index + 1] = 0;
        ++buff_index;
        //printf("%s\n", buff);
    }
    return 0;
}

int translate_input(LineStruct *line_struct)
{
	int error_flag = 0;
	int memory_address = get_var_address(line_struct->basic_operand);
	if (memory_address < 0) {
		char buff[200];
		int num_spaces = calcspaces(line_struct->basic_str, line_struct->basic_operand - line_struct->basic_str);
		sprintf(buff, "var name \"%s\" unacceptable\n%s\n\E[32m\E[1m%*c\E[0m", line_struct->basic_operand, line_struct->basic_str, num_spaces + 1, '^');
		print_error("error", buff);
		error_flag = 1;
	}
	printf("address:%d\n", memory_address);
	
	line_struct->translated_line_num = asm_last_memory_cell;
	AsmCommandPush(line_struct->command_list, COMMAND_READ, memory_address);
	return error_flag;
}

int translate_output(LineStruct *line_struct)
{
	int error_flag = 0;
	int memory_address = get_var_address(line_struct->basic_operand);
	if (memory_address < 0) {
		char buff[200];
		int num_spaces = calcspaces(line_struct->basic_str, line_struct->basic_operand - line_struct->basic_str);
		sprintf(buff, "var name \"%s\" unacceptable\n%s\n\E[32m\E[1m%*c\E[0m", line_struct->basic_operand, line_struct->basic_str, num_spaces + 1, '^');
		print_error("error", buff);
		error_flag = 1;
	}
	printf("address:%d\n", memory_address);
	
	line_struct->translated_line_num = asm_last_memory_cell;
	AsmCommandPush(line_struct->command_list, COMMAND_WRITE, memory_address);
	return error_flag;
}

int translate_let(LineStruct *line_struct)
{
	int error_flag = 0;
	char *token1;
	char *token2;
	char *token3;
	
	char *basic_str = calloc(strlen(line_struct->basic_operand) + 1, sizeof(char));
	strcpy(basic_str, line_struct->basic_operand);
	char *basic_str_parse = basic_str;
	
	char *str_delim[1] = {"="};
	
	int num_token = 0;
	char *code_token = get_token(&basic_str_parse, str_delim, 1);
	
	while (code_token) {
		
		if (num_token == 0) {
			token1 = code_token;
		} else if (num_token == 1) {
			token2 = code_token;
		} else if (num_token == 2) {
			token3 = code_token;
		} else {
			error_flag = 1;
			break;
		}
		code_token = get_token(&basic_str_parse, str_delim, 1);
		num_token++;
	}
	printf("%s, %s, %s\n", token1, token2, token3);
	
	int memory_address = get_var_address(token1);

	line_struct->translated_line_num = asm_last_memory_cell;
	
	AsmCommandList *list = translate_expression(token3);
	
	AsmCommandCat(line_struct->command_list, list);
	
	AsmCommandPush(line_struct->command_list, COMMAND_STORE, memory_address);
	
	
	AsmCommandPrint(line_struct->command_list);
	
	//int memory_address = get_var_address(line_struct->basic_operand);
	/* if (memory_address < 0) {
		char buff[200];
		int num_spaces = calcspaces(line_struct->basic_str_full, line_struct->basic_operand - line_struct->basic_str);
		sprintf(buff, "var name \"%s\" unacceptable\n%s\n\E[32m\E[1m%*c\E[0m", line_struct->basic_operand, line_struct->basic_str_full, num_spaces + 1, '^');
		print_error("error", buff);
		error_flag = 1;
	}
	printf("address:%d\n", memory_address);
	char buff[3];
	sprintf(buff, "%d", memory_address);
	
	line_struct->translated_str = calloc(strlen("WRITE XX") + 1, sizeof(char));
	strcpy(line_struct->translated_str, "WRITE ");
	strcat(line_struct->translated_str, buff);
	line_struct->translated_line_num = asm_last_memory_cell;
	asm_last_memory_cell = asm_last_memory_cell + 1;
	printf("%s\n", line_struct->translated_str); */
	free(basic_str);
	return error_flag;
}

int translate_end(LineStruct *line_struct)
{
	int error_flag = 0;
	line_struct->translated_line_num = asm_last_memory_cell;
	AsmCommandPush(line_struct->command_list, COMMAND_HALT, 0);
	return error_flag;
}

DataList *goto_list = NULL;
void add_undef_goto(LineStruct *line_struct)
{
	if (!goto_list) {
		goto_list = datalist_new();
	}
	datalist_push_back(goto_list, line_struct);
}

void process_undef_goto()
{
	while (1) {
		if (datalist_size(goto_list) > 0) {
			LineStruct *next = datalist_pop_back(goto_list);
			DataList *node = line_struct_list;
			if (!node) {
				break;
			}
			LineStruct *node_val = node->value;
			while (node_val) {
				if (node_val->basic_line_num == atoi(next->basic_operand)) {
					//next->command_list->current.num = next->translated_line_num;
					next->command_list->current.operand = node_val->translated_line_num;
					//printf("!%s %d!\n", node_val->basic_str, node_val->translated_line_num);
					break;
				}
				node = node->next;
				node_val = node->value;
			}
		} else {
			break;
		}
	}
}

int translate_goto(LineStruct *line_struct)
{
	add_undef_goto(line_struct);
	printf("addgoto\n");
	AsmCommandPush(line_struct->command_list, COMMAND_JUMP, 0);
	return 0;
}

int translate_if(LineStruct *line_struct)
{
	int error_flag = 0;
	
	line_struct->translated_line_num = asm_last_memory_cell;
	
	AsmCommandList *list = translate_expression(line_struct->basic_operand);
	
	printf("%s\n", line_struct->basic_operand);
	
	AsmCommandCat(line_struct->command_list, list);
	
	// IF 2 < 5 GOTO 20
	// - 2 + 5 = 3
	// 5 - 2 = 3
	// 3 JNEG 5
	// 4 GOTO 20
	// 5 //any
	AsmCommandList *jmp = 0;
	AsmCommandList *jmp2 = 0;
	if (last_operation == OPERATION_LESS || last_operation == OPERATION_MORE) {
		jmp = AsmCommandPush(line_struct->command_list, COMMAND_JNEG, 0);
		jmp2 = AsmCommandPush(line_struct->command_list, COMMAND_JZ, 0);
	} else if (last_operation == OPERATION_EQUAL) {
		jmp = AsmCommandPush(line_struct->command_list, COMMAND_JNEG, 0);
		AsmCommandPush(line_struct->command_list, COMMAND_MUL, get_num_var_address(-1));
		jmp2 = AsmCommandPush(line_struct->command_list, COMMAND_JNEG, 0);
	}
	
	LineStruct *tmp_line = calloc(1, sizeof(LineStruct));
	tmp_line->basic_str = line_struct->basic_str;
	tmp_line->basic_command = line_struct->basic_command2;
	tmp_line->basic_operand = line_struct->basic_operand2;
	tmp_line->basic_line_num = line_struct->basic_line_num;
	tmp_line->command_list = calloc(1, sizeof(AsmCommandList));
	if (!strcmp(tmp_line->basic_command, "REM")) {
	//line_struct->translated_str = calloc(1, sizeof(char));
	} else if (!strcmp(tmp_line->basic_command, "INPUT")) {
		if (translate_input(tmp_line)) {
			error_flag = 1;
		}
	} else if (!strcmp(tmp_line->basic_command, "OUTPUT")) {
		if (translate_output(tmp_line)) {
			error_flag = 1;
		}
	} else if (!strcmp(tmp_line->basic_command, "GOTO")) {
		if (translate_goto(tmp_line)) {
			error_flag = 1;
		}
	} else if (!strcmp(tmp_line->basic_command, "IF")) {
		if (translate_if(tmp_line)) {
			error_flag = 1;
		}
	} else if (!strcmp(tmp_line->basic_command, "LET")) {
		if (translate_let(tmp_line)) {
			error_flag = 1;
		}
	} else if (!strcmp(tmp_line->basic_command, "END")) {
		if (translate_end(tmp_line)) {
			error_flag = 1;
		}
	}
	
	//printf("che\n");
	
	if (jmp) {
		AsmCommandCat(line_struct->command_list, tmp_line->command_list);
		jmp->current.operand = asm_last_memory_cell;
		printf("%d\n", asm_last_memory_cell);
	}
	if (jmp2) {
		//AsmCommandCat(line_struct->command_list, tmp_line->command_list);
		jmp2->current.operand = asm_last_memory_cell;
		printf("%d\n", asm_last_memory_cell);
	}
	
	AsmCommandPrint(line_struct->command_list);
	
	return error_flag;
}

int translate_basic(LineStruct *line_struct)
{
	if (!line_struct->basic_str) {
		print_error("internal error", "couldnt find basic_str");
		return -1;
	}
	
	int error_flag = 0;
	
	int num_token = 0;
	
	char *basic_str = calloc(strlen(line_struct->basic_str) + 1, sizeof(char));
	strcpy(basic_str, line_struct->basic_str);
	char *basic_str_parse = basic_str;
	
	char *code_token = get_token(&basic_str_parse, line_delimiters, line_delim_size);
	while (code_token) {
		int num_spaces = calcspaces(line_struct->basic_str, code_token - basic_str);
		
		if (num_token == 0) {
			int number;
			if (!tonumber(code_token, &number) || (number - basic_last_num_line <= 0)) {
				char buff[200];
				sprintf(buff, "line number \"%s\" is wrong\n%s\n\E[32m\E[1m^\E[0m", code_token, line_struct->basic_str);
				print_error("error", buff);
				error_flag = 1;
			}
			basic_last_num_line = number;
			line_struct->basic_line_num = number;
		} else if (num_token == 1) {
			if (validate_command(code_token) == -1) {
				char buff[200];
				sprintf(buff, "unknown command \"%s\" line %d\n%s\n\E[32m\E[1m%*c\E[0m", code_token, line_struct->basic_line_num, line_struct->basic_str, num_spaces + 1, '^');
				print_error("error", buff);
				error_flag = 1;
			}
			line_struct->basic_command = code_token;
			
		} else if (num_token == 2) {
			if (validate_operand(code_token) == -1) {
				char buff[200];
				sprintf(buff, "invalid operand \"%s\" not exist\n%s\n\E[32m\E[1m%*c\E[0m", code_token, line_struct->basic_str, num_spaces + 1, '^');
				print_error("error", buff);
				error_flag = 1;
			}
			line_struct->basic_operand = code_token;
		} else if (num_token == 3) {
			if (validate_command(code_token) == -1) {
				char buff[200];
				sprintf(buff, "unknown command \"%s\" line %d\n%s\n\E[32m\E[1m%*c\E[0m", code_token, line_struct->basic_line_num, line_struct->basic_str, num_spaces + 1, '^');
				print_error("error", buff);
				error_flag = 1;
			}
			line_struct->basic_command2 = code_token;
		} else if (num_token == 4) {
			if (validate_operand(code_token) == -1) {
				char buff[200];
				sprintf(buff, "invalid operand \"%s\" not exist\n%s\n\E[32m\E[1m%*c\E[0m", code_token, line_struct->basic_str, num_spaces + 1, '^');
				print_error("error", buff);
				error_flag = 1;
			}
			line_struct->basic_operand2 = code_token;
		} else {
			error_flag = 1;
			break;
		}
		code_token = get_token(&basic_str_parse, line_delimiters, line_delim_size);
		num_token++;
	}

	if (num_token == 0) {
		return -1;
	}
	if (error_flag) {
		return -1;
	}
	
	printf("1)%d 2)%s 3)%s", line_struct->basic_line_num, line_struct->basic_command, line_struct->basic_operand);
	if (line_struct->basic_command2) {
		printf(" 4)%s 5)%s", line_struct->basic_command2, line_struct->basic_operand2);
	}
	printf("\n");
	
	error_flag = 0;
	
	if (!strcmp(line_struct->basic_command, "REM")) {
		//line_struct->translated_str = calloc(1, sizeof(char));
	} else if (!strcmp(line_struct->basic_command, "INPUT")) {
		if (translate_input(line_struct)) {
			error_flag = 1;
		}
	} else if (!strcmp(line_struct->basic_command, "OUTPUT")) {
		if (translate_output(line_struct)) {
			error_flag = 1;
		}
	} else if (!strcmp(line_struct->basic_command, "GOTO")) {
		if (translate_goto(line_struct)) {
			error_flag = 1;
		}
	} else if (!strcmp(line_struct->basic_command, "IF")) {
		if (translate_if(line_struct)) {
			error_flag = 1;
		}
	} else if (!strcmp(line_struct->basic_command, "LET")) {
		if (translate_let(line_struct)) {
			error_flag = 1;
		}
	} else if (!strcmp(line_struct->basic_command, "END")) {
		if (translate_end(line_struct)) {
			error_flag = 1;
		}
	}
	return 0;
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

//count symbols in string
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

typedef struct VarStructChar {
	int asm_address;
	char name;
	struct VarStructChar *next;
} VarStructChar;

typedef struct VarStructNumber {
	int init;
	int asm_address;
	int value;
	struct VarStructNumber *next;
} VarStructNumber;

VarStructChar *var_struct_head = 0;
VarStructChar *tmp_var_struct_head = 0;
VarStructNumber *num_var_struct_head = 0;
int free_asm_address = 99;
int get_var_address(char *cstr)
{
	if (!cstr) {
		return -1;
	}
	if (!(*cstr >= 'A' && *cstr <= 'Z')) {
		return -1;
	}
	
	if (!var_struct_head) {
		var_struct_head = calloc(1, sizeof(VarStructChar));
		var_struct_head->name = *cstr;
		var_struct_head->asm_address = free_asm_address--;
		return var_struct_head->asm_address;
	}
	
	VarStructChar *last_free = 0;
	VarStructChar *free = var_struct_head;
	while (free) {
		if (free->name == *cstr) {
			return free->asm_address;
		}
		last_free = free;
		free = free->next;
	}
	last_free->next = calloc(1, sizeof(VarStructChar));
	last_free->next->name = *cstr;
	last_free->next->asm_address = free_asm_address--;
	return last_free->next->asm_address;
}

int get_num_var_address(int val)
{
	if (!num_var_struct_head) {
		num_var_struct_head = calloc(1, sizeof(VarStructNumber));
		num_var_struct_head->value = val;
		num_var_struct_head->init = 1;
		num_var_struct_head->asm_address = free_asm_address--;
		return num_var_struct_head->asm_address;
	}
	VarStructNumber *pre_last = 0;
	VarStructNumber *free = num_var_struct_head;
	while (free) {
		if (free->init && free->value == val) {
			return free->asm_address;
		}
		pre_last = free;
		free = free->next;
	}
	pre_last->next = calloc(1, sizeof(VarStructNumber));
	pre_last->next->init = 1;
	pre_last->next->value = val;
	pre_last->next->asm_address = free_asm_address--;
	return pre_last->next->asm_address;
}

int get_tmp_var_address(char cstr)
{
	if (!cstr) {
		return -1;
	}
	if (!(cstr >= 'a' && cstr <= 'z')) {
		return -1;
	}
	
	if (!tmp_var_struct_head) {
		tmp_var_struct_head = calloc(1, sizeof(VarStructChar));
		tmp_var_struct_head->name = cstr;
		tmp_var_struct_head->asm_address = free_asm_address--;
		return tmp_var_struct_head->asm_address;
	}
	
	VarStructChar *last_free = 0;
	VarStructChar *free = tmp_var_struct_head;
	while (free) {
		if (free->name == cstr) {
			return free->asm_address;
		}
		last_free = free;
		free = free->next;
	}
	last_free->next = calloc(1, sizeof(VarStructChar));
	last_free->next->name = cstr;
	last_free->next->asm_address = free_asm_address--;
	return last_free->next->asm_address;
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

	int num_line = 0;
	
	int error_flag = 0;
	
	char *saveptr1;
	char *code_line = strtok_r(code_buff, "\n", &saveptr1);
	
	line_struct_list = datalist_new();
	
	while (code_line) {
		LineStruct *line_struct = calloc(1, sizeof(LineStruct));
		line_struct->basic_str = calloc(strlen(code_line) + 1, sizeof(char));
		strcpy(line_struct->basic_str, code_line);
		line_struct->basic_line_num = num_line;
		line_struct->command_list = calloc(1, sizeof(AsmCommandList));
		datalist_push_back(line_struct_list, line_struct);
		
		// analis and fill struct
		if (translate_basic(line_struct)) {
			error_flag = 1;
		}
		
		code_line = strtok_r(0, "\n", &saveptr1);
		num_line++;
	}
	if (error_flag) {
		printf("error\n");
		return -1;
	}
	
	process_undef_goto();

	AsmCommandList *all_command = calloc(1, sizeof(AsmCommandList));
	DataList *node = line_struct_list;
	LineStruct *node_val = node->value;
	while (node_val) {
		AsmCommandCat(all_command, node_val->command_list);
		node = node->next;
		if (!node) {
			break;
		}
		node_val = node->value;
	}
	
	VarStructNumber *num_node = num_var_struct_head;
	while (num_node) {
		if (num_node->init) {
			AsmCommandPushEx(all_command, COMMAND_EQUAL, num_node->value, num_node->asm_address);
		}
		num_node = num_node->next;
	}
	
	int outdesc = open(outfile, O_WRONLY | O_TRUNC | O_CREAT, -1);
	
	if (outdesc == -1) {
		print_error("error", "cant open outfile");
	}
	
	AsmCommandPrintEx(all_command, outdesc);
	
	datalist_free(line_struct_list);
	
	//pairs line_num to fill memory
	
	/* if (write(outdesc, memory_buff, 400) < 0) {
		print_error("error", "cant write in outfine");
		return -1;
	} */
	
	if (close(outdesc) == -1) {
		print_error("error", "cant close outfile");
	}
	return 0;
}
