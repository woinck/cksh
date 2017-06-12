#ifndef _YAUSH_H
#define _YAUSH_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <readline/readline.h>
#include <readline/history.h>

#define PROMPT "yaush> "

extern char * commands[];

typedef struct Arg{
	char * value;
	struct Arg * next;
}Arg;


typedef struct Command{
	char * cmd;
	Arg * arg;
	int argnum;
	int depend; // For && and ||
	char * fout; // For <, |
	int append;
	char * fin; // For >, >>, |
	struct Command * next;
	int pp; // rw
}Command;

typedef struct CommandList{
	Command * command;
	int number;
	int background; // For &
}CommandList;


int get_cmd_number(char *);
CommandList * parse_line(char *);
int execute_cmds(CommandList *);
void print_cmdlist(CommandList *);

#endif
