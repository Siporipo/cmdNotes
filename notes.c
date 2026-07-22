#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "cmdutils.h"
#define CONF_PATH "./noteconf.txt"

int main(int argc, char *argv[]) {
	if (argc < 2) return 0; // print commands
	
	struct command *cmds = command_setup();
	
	// opening and parsing file behind CONF_PATH
	cond_new_file(CONF_PATH);
	FILE *fconf = fopen(CONF_PATH, "r");
	struct fcontents fconfc = get_lines(fconf);
	if (fconfc.lnc == -1) {
		printf("Failed to parse %s\n", CONF_PATH);
		return 0;
	}

	// acting on file load/create via commands 'new' and 'open'
	if (strcmp(argv[1], "new") == 0) {
		if (argc < 3) {
			printf("Too little arguments, missing new file path(-s)!\n");
			goto EXIT_FCONF;
		}
		for (int i = 0; i < argc - 2; i++) {
			cond_new_file(argv[i+2]);
		}
		goto EXIT_FCONF;
	}
	
	if (strcmp(argv[1], "open") == 0) {
		if (argc > 3) {
			printf("You cannot open more than once file at once!\n");
			goto EXIT_FCONF;
		}
		if (file_exists(argv[2]) == 0) {
			printf("Selected file not found, please enter a valid file path!\n");
			goto EXIT_FCONF;
		}
		freopen(NULL, "w", fconf);
		fputs(argv[2], fconf);
		fputc('\n', fconf);
		for (int i = 1; i < fconfc.lnc; i++) {
			fputs(fconfc.lines[i], fconf);
			fputc('\n', fconf);
		}
		goto EXIT_FCONF;
	}
	goto SECTION_FP;

EXIT_FCONF:
	fclose(fconf);
	return 0;

SECTION_FP: ;	
	// made listening to "You are not alone" by MJ, now my favourite song ever :)
	size_t strlen_fp = strlen(fconfc.lines[0]);

	char *filepath = malloc(strlen_fp);
	if (filepath) {
		memcpy(filepath, fconfc.lines[0], strlen_fp);
		filepath[strlen_fp - 1] = '\0';
	}
	
	if (file_exists(filepath) == 0) {
		printf("No valid file has been selected!\n");
		goto EXIT_FCONF;
	}
	goto SECTION_FPTR;

EXIT_FP_FCONF:
	free(filepath);
	goto EXIT_FCONF;

SECTION_FPTR: ;
	FILE *fptr = fopen(filepath, "r");
	if (fptr == NULL) {
		printf("Could not open file.\n");
		goto EXIT_FP_FCONF;
	}

	int cmdn;
	for (cmdn = 0; cmdn < CMDCOUNT; cmdn++) {
		if (strcmp(argv[1], cmds[cmdn].name) == 0) {
			cmds[cmdn].fnptr(argc, argv, fptr);
			goto EXIT_FPTR_FP_FCONF;
		}
	}

EXIT_FPTR_FP_FCONF:
	free(filepath);
	fclose(fptr);
	fclose(fconf);
	return 0;
}
