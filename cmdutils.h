#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CMDCOUNT 4

struct command {
	char *name;
	void (*fnptr)(int, char**, FILE*);
};

struct fcontents {
	int lnc;
	char **lines;
};

struct  command *command_setup();

struct  fcontents get_lines(  FILE *fptr  ); 
int     file_exists(          char *path  );
void    cond_new_file(        char *path  );

void    cmd_list(   int argc, char **argv, FILE *fptr  );
void    cmd_add(    int argc, char **argv, FILE *fptr  );
void    cmd_clear(  int argc, char **argv, FILE *fptr  );
void    cmd_remove( int argc, char **argv, FILE *fptr  );
