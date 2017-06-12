#include "yaush.h"

#define STATE_START 0
#define STATE_AND 1
#define STATE_OR 2
#define STATE_IN 3
#define STATE_OUT 4
#define STATE_APPEND 5
#define STATE_PIPE 6
#define STATE_ARG 7

char * commands[]={"pwd","cd", "delete", "help", "ls", "rename", "stat"};

CommandList * new_command_list()
{
	CommandList * clist = malloc(sizeof(CommandList));
	clist->command = malloc(sizeof(Command));
	clist->number = 0;
	clist->background = 0;
	return clist;
}

Command * new_command()
{
	Command * cmd = malloc(sizeof(Command));
	cmd->arg = NULL;
	cmd->argnum = 0;
	cmd->depend = 0; // For && and ||
	cmd->fout = STDOUT_FILENO; // For <, |
	cmd->fin = STDIN_FILENO; // For >, >>, |
	cmd->next = NULL;
	return cmd;
}

void next_cmd(Command ** curcmd, Arg ** curarg, char * word)
{
	(*curcmd)->next = new_command();
	(*curcmd) = (*curcmd)->next;
	(*curcmd)->next = NULL;
	(*curcmd)->arg = malloc(sizeof(Arg));
	(*curcmd)->cmd = word;
	(*curcmd)->argnum = 1;

	*curarg = (*curcmd)->arg;
	(*curarg)->value = word;
	(*curarg)->next = NULL;
}
char * nextword(char * s)
{
	static char * ptr;
	if(s!=NULL) ptr = s;
	char * start = ptr;
	char * result;
	if (*ptr=='\0') 
	{
		return NULL;
	}
	int escape = 0;
	for (;;ptr++)
	{
		if (*ptr=='\0')
		{
			result = malloc(sizeof(char) * (ptr - start + 1));
			strncpy(result, start, ptr-start+1);
			return result;
		}

		if (escape == 1)
		{
			escape = 0;
			continue;
		}
		else if (escape == 2)
		{
			if (*ptr == '\'') escape = 0;
			continue;
		}
		else if (escape == 3)
		{
			if (*ptr == '\"') escape = 0;
			continue;
		}
		
		if (*ptr == '\\')
			escape = 1;
		else if (*ptr == '\'')
			escape = 2;
		else if (*ptr == '\"')
			escape = 3;
		else if (*ptr == ' ')
		{
			result = malloc(sizeof(char) * (ptr - start + 1));
			strncpy(result, start, ptr-start);
			result[ptr-start] = '\0';
			ptr++;
			return result;
		}
	}	
}

CommandList * parse_line(char * line)
{
	char * s = strdup(line);
	//printf("parsing line: %s\n",line);
	//int length = strlen(s);
	CommandList * clist = new_command_list();

	//clist.command = new_command();
	Command * curcmd = clist->command;
	Arg * curarg = NULL;

	int state = STATE_START;
	char * word;
	
	word = nextword(s);
	while (word != NULL)
	{
		
		if (!strcmp(word, "&&"))
		{
			state = STATE_AND;
		}
		else if (!strcmp(word, "||"))
		{
			state = STATE_OR;
		}
		else if (!strcmp(word, "<"))
		{
			state = STATE_IN;
		}
		else if (!strcmp(word, ">"))
		{
			state = STATE_OUT;
		}
		else if (!strcmp(word, ">>"))
		{
			state = STATE_APPEND;
		}
		else if (!strcmp(word, "|"))
		{
			state = STATE_PIPE;
			int fd[2];
			pipe(fd);
			curcmd->pp = curcmd->pp | 1;
			curcmd->fout = fd[1];
		}
		else if (!strcmp(word, "&"))
		{
			clist->background = 1;
		}
		else
		{
			switch(state)
			{
				case STATE_START:
					{
						next_cmd(&curcmd, &curarg, word);
						state = STATE_ARG;
						break;
					}
				case STATE_AND:
					{
						next_cmd(&curcmd, &curarg, word);
						curcmd->depend = 1;
						state = STATE_ARG;
						break;
					}
				case STATE_ARG:
					{
						curcmd->argnum++;
						curarg->next = malloc(sizeof(Arg));
						curarg = curarg->next;
						curarg->value = word;
						curarg->next = NULL;
						break;
					}
				default:
					break;
			}
		}
		
		word = nextword(NULL);

	}

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
int check_clist(CommandList clist)
{
	return 1;
}
/*
CommandList parse_line1(char * line)
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
}*/

void print_cmd(Command * cmd)
{
	printf("cmd:%s\n",cmd->cmd);
	Arg * curarg = cmd->arg;
	printf("arg:\n");
	while (curarg != NULL)
	{
		printf("%s\n",curarg->value);
		curarg = curarg->next;
	}
	return;
}
void print_cmdlist(CommandList * clist)
{
	printf("printing cmdlist:\n");
	Command * curcmd = clist->command->next;
	printf("background: %d\n", clist->background);
	while (curcmd != NULL)
	{
		print_cmd(curcmd);
		curcmd = curcmd->next;
	}
	return;
}
