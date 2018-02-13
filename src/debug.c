#include <stdio.h>
#include "controldevice.h"
#include "debug.h"


void sm_printMemory()
{
	int buf = 0;
	printf("%7d", 0);
	for (int i = 1; i < 10; i++) {
		printf("%6d", i);
	}
	printf("\n");
	for (int i = 0; i < 10; i++) {
		printf("%d", i);
		if (sc_memoryGet(i * 10, &buf)) {
			printf("\n error read memory addres: %d\n", i * 10);
		}
		printf(" +%04d ", buf);
		for (int j = 1; j < 10; j++) {
			if (sc_memoryGet(i * 10 + j, &buf)) {
				printf("\n error read memory addres: %d\n", i * 10 + j);
			}
			printf("+%04d ", buf);
		}
		printf("\n");
	}
}

void sm_printFlags()
{
	printf("CDMIE\n");
	for (int i = 0; i < 5; i++) {
		int val = 0;
		sc_regGet(i, &val);		
		printf("%d", val);
	}
	printf("\n");
}
