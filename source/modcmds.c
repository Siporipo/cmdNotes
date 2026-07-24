#include "modcmds.h"
#include "cmdutils.h"
#include <math.h>

void cmd_list(int argc, char **argv, FILE *fptr) {
	struct fcontents fc = get_lines(fptr);
	if (!fc.lines && fc.lnc == -1) {
		printf("get_lines() failed\n");
		return;
	}
	if (fc.lnc == 0) {
		printf("NO LINES TO DISPLAY\n");
		return;
	}
	int padding = ceil(log10(fc.lnc)) + 1;
	for (int i = 0; i < fc.lnc; i++) {
		printf("%-*d %s", padding, i + 1, fc.lines[i]);
		free(fc.lines[i]);
	}
	free(fc.lines);
}

void cmd_add(int argc, char **argv, FILE *fptr) {
	if (argc < 3) {
		printf("Please provide an item to add!\n");
		return;
	}

	struct fcontents fc = get_lines(fptr);
	if (fc.lines == NULL && fc.lnc == -1) {
		printf("get_lines() failed\n");
		return;
	}
	freopen(NULL, "w", fptr);

	// restore previous lines if previous lines existed
	if (fc.lnc > 0) {
		for (int i = 0; i < fc.lnc; i++) {
			fputs(fc.lines[i], fptr);
			free(fc.lines[i]);
		}
		fseek(fptr, -1, SEEK_CUR);
		if (fgetc(fptr) != '\n') {
			fputc('\n', fptr);
		}
	}
	
	for (int argn = 2; argn < argc; argn++) {
		fputs(argv[argn], fptr);
		fputc('\n', fptr);
	}
}

void cmd_clear(int argc, char **argv, FILE *fptr) {
	freopen(NULL, "w", fptr);
}

void cmd_remove(int argc, char **argv, FILE *fptr) {
	if (argc < 3) {
		printf("Please provide at least one line # to remove!\n");
		return;
	}

	struct fcontents fc = get_lines(fptr);
	if (fc.lines == NULL && fc.lnc == -1) {
		printf("get_lines() failed\n");
		return;
	}
	freopen(NULL, "w", fptr);

	int rems[argc - 2];
	for (int i = 0; i < argc - 2; i++) {
		int stint = atoi(argv[i+2]);
		if (stint > fc.lnc || stint < 0) {
			rems[i] = -1;
		} else {
			rems[i] = stint - 1;
		}
	}

	if (fc.lnc > 0) {
		int newlnc = 0;
		for (int i = 0; i < fc.lnc; i++) {
			int ok = 1;
			for (int j = 0; j < argc - 2; j++) {
				if (i == rems[j]) {
					ok--;
				}
			}
			if (ok == 1) {
				newlnc++;
				fputs(fc.lines[i], fptr);
				free(fc.lines[i]);
			}
		}
	}
}


struct command *modcmd_setup() {
	struct command *cmds = malloc(CMDCOUNT * sizeof *cmds);
	cmds[0] = (struct command){
		.name="list",
		.fnptr=&cmd_list,
		.help="Lists selected file contents in a numbered list fashion.\nOutputs NO LINES IN BUFFER if a valid, empty file is selected."
	};
	cmds[1] = (struct command){
		.name="add",
		.fnptr=&cmd_add,
		.help="Writes n arguments, each in a new line."
	};
	cmds[2] = (struct command){
		.name="clear",
		.fnptr=&cmd_clear,
		.help="Wipes the selected file."
	};
	cmds[3] = (struct command){
		.name="remove",
		.fnptr=&cmd_remove,
		.help="Removes n arguments by hash listed by `list`."
	};
	return cmds;
}
