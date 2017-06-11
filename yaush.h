#ifndef _YAUSH_H
#define _YAUSH_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <readline/readline.h>
#include <readline/history.h>

#define PROMPT "yaush> "

extern char * commands[];

typedef struct Command{
	char * cmd;
	char ** arg;
	int depend; // For && and ||
	int fout; // For <, |
	int fin; // For >, >>, |
}Command;

typedef struct CommandList{
	Command * command;
	int number;
	int background; // For &
}CommandList;


int get_cmd_number(char *);
CommandList parse_line(char *);
int execute_cmds(CommandList *);

#endif
