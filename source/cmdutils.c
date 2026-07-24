#include "cmdutils.h"
#include <math.h>

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
			if (c != '\0') lnbuf[col] = '\0';
			
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

			size_t full_lnlen = strlen(lnbuf) + 1;
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
