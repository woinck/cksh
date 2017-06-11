#include "yaush.h"

char * commands[]={"pwd","cd", "delete", "help", "ls", "rename", "stat"};

CommandList new_command_list()
{
	CommandList clist;
	clist.number = 0;
	clist.background = 0;
	clist.command = NULL;
	return clist;
}

int get_cmd_number(char * s)
{
	int linelen = strlen(s);
	int i = 0, flag = 0;
	int num = 0;
	for (i=0;i<linelen;++i)
	{
		if (s[i]!='|' && s[i]!='&' && s[i]!=' ' && flag == 0)
		{
			flag = 1;
			num++;
		}
		else if ((s[i]=='|' || s[i]=='&') && flag == 1)
		{
			flag = 0;
			if (s[i]=='&' && s[i+1]!='&')
			{
				perror("invalid symbol '&'");
				return -1;
			}
			if (s[i+1] == s[i])
				++i;
		}
		else if ((s[i] == '|' || s[i]=='&') && flag == 0) // parse error
		{
			perror("parse error");
			return -1;
		}
	}
	return num;
}

CommandList parse_line(char * line)
{
	char * s = strdup(line);
	int i=0;
	CommandList clist = new_command_list();
	int linelen = strlen(s);
	if (linelen >= 2 && line[linelen - 1] == '&' && line[linelen - 2] != '&')
	{
		clist.background = 1;
		s[linelen - 1] = '\0';
	}
	else
		clist.background = 0;
	clist.number = get_cmd_number(s);

	clist.command = malloc(clist.number * sizeof(Command));

	char * cmd;
	cmd = strtok(s, "&|");
	while (cmd != NULL)
	{
		parse_cmd(clist.command + i, cmd);
	}
	return clist;
}

int check_clist(CommandList clist)
{
	return 1;
}

