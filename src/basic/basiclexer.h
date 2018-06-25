#ifndef BASIC_LEXER_H
#define BASIC_LEXER_H

#define COMMAND_EQUAL 999

typedef struct {
	int num;
	int command;
	int	operand;
} AsmCommand;

typedef struct AsmCommandList {
	int init;
	AsmCommand current;
	struct AsmCommandList *next;
} AsmCommandList;

typedef struct {
	int basic_line_num;
	int translated_line_num;
	char *basic_str;
	char *basic_str_full;
	char *basic_command;
	char *basic_operand;
	char *basic_command2;
	char *basic_operand2;
	AsmCommandList *command_list;
} LineStruct;

int get_var_address(char *cstr);
int get_tmp_var_address(char cstr);
int get_num_var_address(int val);
char *get_token(char **str_p, char *delimiters[], int size);
int lexer(char *infile, char *outfile);
int tonumber(char *s, int *number);
int calcspaces(char *code_token, int num);
int translate_basic(LineStruct *line_struct);
int validate_operand(char *cstr);
int validate_command(char *cstr);

#endif