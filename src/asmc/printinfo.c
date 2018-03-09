#include <stdio.h>
#include "printinfo.h"

void print_help()
{
	printf("Simple Assambler Translator\n\nUsage: sat infile [outfile]\n");
}

void print_version()
{
	printf("sat 0.1v\n");
}

void print_error(char *type, char *text)
{
	printf("\E[1msat: \E[31m%s: \E[0m%s\n", type, text);
}

void print_no_input_files()
{
	print_error("fatal error", "no input files\ntranslation terminated.\n");
}
