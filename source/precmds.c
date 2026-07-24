#include "precmds.h"

void cmd_help() {
	if (file_exists(HELP_PATH) == 0) {
		printf("Could not locate %s\n", HELP_PATH);
		return;
	}
	FILE *hptr = fopen(HELP_PATH, "r");
	if (!hptr) {
		printf("Failed to open %s\n", HELP_PATH);
		return;
	}
	struct fcontents hc = get_lines(hptr);
	if (!hc.lines && hc.lnc == -1) {
		printf("Failed to parse %s\n", HELP_PATH);
		goto EXIT_HPTR;
	}
	for (int i = 0; i < hc.lnc; i++) {
		printf("%s", hc.lines[i]);
		free(hc.lines[i]);
	} free(hc.lines);
	
EXIT_HPTR:
	fclose(hptr);
	return;
}

void cmd_help_cmds(char **argv, struct command *modcmds, struct command *precmds) {
	char *arg = NULL;
	if (strcmp(argv[1], "--help") == 0) {
		size_t arglen = strlen(argv[2]) + 1;
		arg = malloc(arglen);
		if (!arg) {
			printf("malloc failed to allocate memory to arg\n");
			return;
		}
		memcpy(arg, argv[2], arglen);
	} else {
		size_t arglen = strlen(argv[1]) + 1;
		arg = malloc(arglen);
		if (!arg) {
			printf("malloc failed to allocate memory to arg\n");
			return;
		}
		memcpy(arg, argv[1], arglen);
	}
	// starting with modifyer commands
	for (int i = 0; i < CMDCOUNT; i++) {
		if (strcmp(arg, modcmds[i].name) == 0) {
			printf("%s - %s\n", modcmds[i].name, modcmds[i].help);
			goto EXIT_0;
		}
	}
	// now pre-mod commands
	for (int i = 0; i < CMDCOUNT; i++) {
		if (strcmp(arg, precmds[i].name) == 0) {
			printf("%s - %s\n", precmds[i].name, precmds[i].help);
			goto EXIT_0;
		}
	}

EXIT_0:
	free(arg);
	return;
}

void cmd_delete(int argc, char **argv, FILE *fptr) {
	if (argc == 2) {
		goto DEL_OPEN;
	} else {
		goto DEL_ARGS;
	}
DEL_OPEN: ;
	struct fcontents fconfc = get_lines(fptr);
	if (fconfc.lnc == -1) {
		printf("Failed to parse %s\n", CONF_PATH);
		return;
	}
	if (fconfc.lnc == 0) {
		printf("No file has been selected for modification, thus deletion with no args\n");
		goto EXIT_FCONF;
	}
	if (file_exists(fconfc.lines[0]) == 0) {
		printf("Currently open file does not exist!\n");
		goto EXIT_FCONF;
	}
	if (remove(fconfc.lines[0]) != 0) {
		printf("Failed to remove file %s\n", fconfc.lines[0]);
		goto EXIT_FCONF;
	} goto EXIT_FCONF;

DEL_ARGS: ;
	int errc = 0;
	for (int i = 2; i < argc; i++) {
		if (file_exists(argv[i]) == 0) {
			if (errc == 0) printf("ERROR:\n");
			printf("	file path no. %d does not exist\n", i - 1);
			errc++;
		}
	}
	if (errc > 0) return;
	for (int i = 2; i < argc; i++) {
		if (remove(argv[i]) != 0) {
			printf("Failed to remove file at: %s\n", argv[i]);
		}
	} goto EXIT_0;

EXIT_FCONF:
	for (int i = 0; i < fconfc.lnc; i++) {
		free(fconfc.lines[i]);
	} free(fconfc.lines);
	return;
EXIT_0:
	printf("WARNING:\n	If you have deleted a selected file, if you try to operate with it in any way, an error will show saying -\n	`No valid file has been selected!`\n");
	return;
}

void cmd_new(int argc, char **argv, FILE *fptr) {
	struct fcontents fconfc = get_lines(fptr);
	if (fconfc.lnc == -1) {
		printf("Failed to parse %s\n", CONF_PATH);
		return;
	}
	if (argc < 3) {
		printf("Too little arguments, missing new file path(-s)!\n");
		goto EXIT_FCONF;
	}
	for (int i = 0; i < argc - 2; i++) {
		cond_new_file(argv[i+2]);
	}
EXIT_FCONF:
	for (int i = 0; i < fconfc.lnc; i++) {
		free(fconfc.lines[i]);
	} free(fconfc.lines);
	return;
}
	
void cmd_open(int argc, char **argv, FILE *fptr) {
	struct fcontents fconfc = get_lines(fptr);
	if (fconfc.lnc == -1) {
		printf("Failed to parse %s\n", CONF_PATH);
		return;
	}
	if (argc > 3) {
		printf("You cannot open more than once file at once!\n");
		goto EXIT_FCONF;
	}
	if (file_exists(argv[2]) == 0) {
		printf("Selected file not found, please enter a valid file path!\n");
		goto EXIT_FCONF;
	}
	freopen(NULL, "w", fptr);
	
	char *fpptr = realpath(argv[2], NULL);
	if (!fpptr) {
		printf("Failed to find real path.\nERRO CODE: %d\n", errno);
		goto EXIT_FCONF;
	}
	fputs(fpptr, fptr);
	free(fpptr);

	fputc('\n', fptr);
	for (int i = 1; i < fconfc.lnc; i++) {
		fputs(fconfc.lines[i], fptr);
		fputc('\n', fptr);
	}
EXIT_FCONF:
	for (int i = 0; i < fconfc.lnc; i++) {
		free(fconfc.lines[i]);
	} free(fconfc.lines);
	return;
}

void cmd_opened(int argc, char **argv, FILE *fptr) {
	struct fcontents fconfc = get_lines(fptr);
	if (fconfc.lnc == -1) {
		printf("Failed to parse %s\n", CONF_PATH);
		return;
	}
	printf("%s", fconfc.lines[0]);

	// Exit code
	for (int i = 0; i < fconfc.lnc; i++) {
		free(fconfc.lines[i]);
	} free(fconfc.lines);
	return;
}


struct command *precmd_setup() {
	struct command *cmds = malloc(PRECMDCOUNT * sizeof *cmds);
	cmds[0] = (struct command){
		.name="new",
		.fnptr=cmd_new,
		.help="Creates new file, does not automatically open it."
	};
	cmds[1] = (struct command){
		.name="delete",
		.fnptr=cmd_delete,
		.help="Deletes files specified in [<args>]\nTries to delete the open file if no argument is provided."
	};
	cmds[2] = (struct command){
		.name="open",
		.fnptr=cmd_open,
		.help="Opens one file if the path is valid and accessible."
	};
	cmds[3] = (struct command){
		.name="opened",
		.fnptr=cmd_opened,
		.help="Outputs selected/opened file path."
	};
	return cmds;
}
