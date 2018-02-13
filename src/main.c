#include <stdio.h>
#include <stdlib.h>
#include "controldevice.h"
#include "debug.h"

int main()
{
	sc_memoryInit();
	// set in memory multiplication table
	printf("TEST MEMORY OUTPUT\n");
	for (int i = 0; i < 10; i++) { 
		for (int j = 0; j < 10; j++) {
			sc_memorySet(i * 10 + j, i * j);
		}
	}
	// set in memory multiplication table
	sm_printMemory();
	
	// save table
	printf("TEST MEMORY SAVE\n");
	if (sc_memorySave("mul_table.bin")) {
		printf("\n error memory save \n");
	}
	// clear memory
	sc_memoryInit();
	sm_printMemory();
	
	// load table
	printf("TEST MEMORY LOAD\n");
	if (sc_memoryLoad("test.bin")) {
		printf("\n error memory load \n");
	}
	system("clear");
	sm_printMemory();

	sc_regInit();
	// check flags
	printf("TEST MEMORY SET\n");
	sm_printFlags();
	sc_memorySet(100, 9999);
	sm_printFlags();
	
	sc_regSet(MF, 0);
	printf("TEST MEMORY GET\n");
	int val1 = 0;
	sm_printFlags();
	sc_memoryGet(100, &val1);
	printf("%d\n", val1);
	sm_printFlags();
	
	printf("\n");
	val1 = 0;
	sm_printFlags();
	sc_memoryGet(99, &val1);
	printf("%d\n", val1);
	sm_printFlags();
	
	//check command encode/decode
	printf("TEST COMMAND ENCODE\n");
	int val2 = 0;
	if (sc_commandEncode(76, 127, &val2)) {
		printf("\n error memory encode \n");
	}
	printf("command:76, operand:127 encoded:%X\n", val2);
	int command = 0;
	int operand = 0;
	sc_commandDecode(val2, &command, &operand);
	printf("encoded:%X, command:%d operand:%d\n", val2, command, operand);
	return 0;
}
