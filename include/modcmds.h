#pragma once
#include "cmdutils.h"
#include <math.h>

void cmd_add(		int argc, char **argv, FILE *fptr	);
void cmd_remove(	int argc, char **argv, FILE *fptr	);
void cmd_clear(		int argc, char **argv, FILE *fptr	);
void cmd_list(		int argc, char **argv, FILE *fptr	);

struct command *modcmd_setup();
