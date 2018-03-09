#include <stdio.h>
#include <unistd.h>
#include <string.h>

void print_help()
{
	printf("Simple Assambler Translator\n\nUsage: sat infile [outfile]\n");
}

void print_version()
{
	printf("sat 0.1v\n");
}

int main(int argc, char **argv)
{
	if (argc == 3) {
		if(access("termsettings", F_OK) == -1) {
			//rk_mytermsave();
		}
	} else if (argc == 2) {
		if (!strcmp(argv[1], "-h") || !strcmp(argv[1], "--help")) {
			print_help();
		}
		if (!strcmp(argv[1], "--version")) {
			print_version();
		}
	} else {
		printf("\E[1msat: \E[31mfatal error: \E[0mno input files\ntranslation terminated.\n");
		return -1;
	}
	return 0;
}
