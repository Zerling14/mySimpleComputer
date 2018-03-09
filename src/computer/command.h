#ifndef COMMAND_H_GUARD
#define COMMAND_H_GUARD

//	I/O
#define COMMAND_READ 	10
#define COMMAND_WRITE	11
//	LOAD/STORE
#define COMMAND_LOAD	20
#define COMMAND_STORE	21
//	ARITHMETIC
#define COMMAND_ADD		30
#define COMMAND_SUB		31
#define COMMAND_DIVIDE	32
#define COMMAND_MUL		33
//	TRANSFER OF CONTROL
#define COMMAND_JUMP	40
#define COMMAND_JNEG	41
#define COMMAND_JZ		42
#define COMMAND_HALT	43
//	USER FUNCTIONS
#define COMMAND_NOT		51
#define COMMAND_AND		52
#define COMMAND_OR		53
#define COMMAND_XOR		54
// MY USER FUNCTIONS
#define COMMAND_SET		77
#define COMMAND_NOP		78

extern int processor_commands[13];
extern int user_commands_start;
extern int user_commands_end;

#endif
