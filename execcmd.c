#include "yaush.h"

char ** argstr(Command * cmd)
{
	int i=0;
	char ** str = malloc(sizeof(char *) * (cmd->argnum + 1));
	Arg * curarg = cmd->arg;
	for (i=0;i<cmd->argnum;++i)
	{
		str[i] = curarg->value;
		curarg = curarg->next;
	}
	str[cmd->argnum]=NULL;
	return str;
}

int execute_cmd(Command * cmd, int prevret)
{
	char ** arglist = argstr(cmd);
	if (execvp(cmd->cmd, arglist)==-1)
		return errno;
	else
		return 0;
}

int execute_cmds(CommandList * clist)
{
	//print_cmdlist(clist);
	Command * curcmd = clist->command->next;
	int ret = 0;
	while(curcmd!= NULL)
	{
		ret = execute_cmd(curcmd, ret);
	}
	return ret;
}
