#pragma once
#include "cmdutils.h"

void cmd_help();
void cmd_help_cmds(char **argv, struct command *modcmds, struct command *precmds);

void cmd_new(		int argc, char **argv, FILE *fptr	);
void cmd_delete(	int argc, char **argv, FILE *fptr	);
void cmd_open(		int argc, char **argv, FILE *fptr	);
void cmd_opened(	int argc, char **argv, FILE *fptr	);

struct command *precmd_setup();
