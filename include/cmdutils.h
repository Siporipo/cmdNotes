#pragma once

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define CMDCOUNT 4
#define PRECMDCOUNT 4

#define CONF_PATH "./noteconf.txt"
#define HELP_PATH "./notehelp.txt"

struct command {
	char *name;
	void (*fnptr)(int, char**, FILE*);
	char *help;
};

struct fcontents {
	int lnc;
	char **lines;
};

struct  fcontents get_lines(  FILE *fptr  ); 
int     file_exists(          char *path  );
void    cond_new_file(        char *path  );
