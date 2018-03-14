#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "lexer.h"
#include "printinfo.h"

int main(int argc, char **argv)
{
	char infile[20];
	char outfile[20] = "default.o";
	
	if (argc == 3 || argc == 2) {
		if (!strcmp(argv[1], "-h") || !strcmp(argv[1], "--help")) {
			print_help();
			return 0;
		} else if (!strcmp(argv[1], "--version")) {
			print_version();
			return 0;
		}
		if(access(argv[1], F_OK) == -1) {
			char buff[40];
			strncpy(buff, argv[1], 39);
			buff[39] = 0;
			strncat(buff, ": File is missing", 39);
			buff[39] = 0;
			print_error("error", buff);
			print_no_input_files();
			return -1;
		}
	} else {
		print_no_input_files();
		return -1;
	}

	strncpy(infile, argv[1], 19);
	infile[19] = 0;
	if (argc == 3) {
		strncpy(outfile, argv[2], 19);
		infile[19] = 0;
	}
	
	if (lexer(infile, outfile)) {
		return -1;
	}
	
	//printf("%s, %s\n", infile, outfile);
	return 0;
}
