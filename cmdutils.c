#include "cmdutils.h"

//===== HELPER FUNCTIONS =====//
struct fcontents get_lines(FILE *fptr) {
	// This function skips multiplying by sizeof char 
	// because it is redundant and causes readable overhead

	freopen(NULL, "r", fptr);
	fseek(fptr, 0, SEEK_SET);
	int row = 0;
	int col = 0;
	
	size_t lnsz   = 128u;
	char  *lnbuf  = malloc(lnsz);
	char **retbuf = NULL;

	int c = fgetc(fptr);
	while (c != EOF) {
		if (lnsz - 1 <= col) {
			char *temp = realloc(lnbuf, lnsz * 2);
			if (!temp) {
				for (int i = 0; i <= row; i++) {
					free(retbuf[i]);
				}
				free(retbuf);
				free(lnbuf);
				return (struct fcontents){ .lnc=-1, .lines=NULL };
			} else {
				lnbuf = temp;
			}
			lnsz *= 2;
		}
		lnbuf[col++] = c;
		
		if (c == '\n' || c == '\0') {
			if (c == '\0') lnbuf[col] = '\0';
			
			char **temp = realloc(retbuf, (row + 1) * sizeof *temp);
			if (!temp) {
				free(lnbuf);
				for (int i = 0; i <= row; i++) {
					free(retbuf[i]);
				}
				free(retbuf);
				return (struct fcontents){ .lnc=-1, .lines=NULL };
			} else {
				retbuf = temp;
			}

			size_t full_lnlen = col + 1;
			retbuf[row] = malloc(full_lnlen);
			memcpy(retbuf[row], lnbuf, full_lnlen);
			memset(lnbuf, 0, lnsz);
			
			col = 0;
			row++;
		}
		c = fgetc(fptr);
	}
	free(lnbuf);
	return (struct fcontents){ .lnc=row, .lines=retbuf };
}

int file_exists(char *path) {
	// returns 1 on success and 0 on fail
	FILE *fptr;
	fptr = fopen(path, "r");
	if (!fptr) {
		return 0;
	}
	fclose(fptr);
	return 1;
}

void cond_new_file(char *path) {
	if (file_exists(path) == 0) {
		FILE *f = fopen(path, "w+");
		fclose(f);
	}
}

struct command *command_setup() {
	struct command *cmds = malloc(CMDCOUNT * sizeof *cmds);
	cmds[0] = (struct command){ .name="list",   .fnptr=&cmd_list   };
	cmds[1] = (struct command){ .name="add",    .fnptr=&cmd_add    };
	cmds[2] = (struct command){ .name="clear",  .fnptr=&cmd_clear  };
	cmds[3] = (struct command){ .name="remove", .fnptr=&cmd_remove };
	return cmds;
}

//===== CMD FUNCTIONS =====//
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
	for (int i = 0; i < fc.lnc; i++) {
		printf("%-3d %s", i + 1, fc.lines[i]);
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
		for (int i = 0; i < fc.lnc; i++) {
			int ok = 1;
			for (int j = 0; j < argc - 2; j++) {
				if (i == rems[j]) {
					ok--;
				}
			}
			if (ok == 1) {
				fputs(fc.lines[i], fptr);
				free(fc.lines[i]);
			}
		}
		fseek(fptr, -1, SEEK_CUR);
		if (fgetc(fptr) != '\n') {
			fputc('\n', fptr);
		}
	}
}
