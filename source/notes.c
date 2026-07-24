#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "cmdutils.h"
#include "modcmds.h"
#include "precmds.h"

int main(int argc, char *argv[]) {
	if (argc < 2) {
		goto CMD_HELP;
	}
	if (strcmp(argv[1], "--help") == 0 && argc == 2) {
		goto CMD_HELP;
	}
	goto SECTION_CMDS;
	
CMD_HELP:
	cmd_help();
	return 0;
	
SECTION_CMDS: ; 
	struct command *cmds    = modcmd_setup();
	struct command *precmds = precmd_setup();
	struct fcontents fconfc; 

	if (argc == 3) {
	if (
		strcmp(argv[1], "--help") == 0 ||
		 strcmp(argv[2], "--help") == 0
	) {
		cmd_help_cmds(argv, cmds, precmds);
		return 0;
	}
	}
	
	cond_new_file(CONF_PATH);
	FILE *fconf = fopen(CONF_PATH, "r");

	for (int i = 0; i < PRECMDCOUNT; i++) {
		if (strcmp(precmds[i].name, argv[1]) == 0) {
			precmds[i].fnptr(argc, argv, fconf);
			fclose(fconf);
			return 0;
		}
	}
	goto SECTION_FP;

EXIT_FCONF:
	for (int i = 0; i < fconfc.lnc; i++) {
		free(fconfc.lines[i]);
	} free(fconfc.lines);
	fclose(fconf);
	return 0;

SECTION_FP: ;	
	fconfc = get_lines(fconf);
	if (fconfc.lnc == -1) {
		printf("Failed to parse %s\n", CONF_PATH);
		return 0;
	}

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
	printf("WARNING: Command `%s` not found!\n\n", argv[1]);
	cmd_help();

EXIT_FPTR_FP_FCONF:
	free(filepath);
	fclose(fptr);
	fclose(fconf);
	return 0;
}
