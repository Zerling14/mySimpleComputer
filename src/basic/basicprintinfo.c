#include <stdio.h>
#include "basicprintinfo.h"

void print_help()
{
	printf("Simple Basic Translator\n\nUsage: basic infile [outfile]\n");
}

void print_version()
{
	printf("basic 0.1v\n");
}

void print_error(char *type, char *text)
{
	printf("\E[1mbasic: \E[31m%s: \E[0m%s\n", type, text);
}

void print_no_input_files()
{
	print_error("fatal error", "no input files\ntranslation terminated.\n");
}
