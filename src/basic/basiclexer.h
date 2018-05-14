#ifndef BASIC_LEXER_H
#define BASIC_LEXER_H

typedef struct {
	int basic_line_num;
	int translated_line_num;
	char *basic_str;
	char *basic_str_full;
	char *basic_command;
	char *basic_operand;
	char *translated_str;
} LineStruct;

int get_var_address(char *cstr);
int validate_operand(char *cstr);
int validate_command(char *cstr);
int tonumber(char *s, int *number);
int lexer(char *infile, char *outfile);
int calcspaces(char *code_token, int num);
int translate_basic(LineStruct *line_struct);

#endif
