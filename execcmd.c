#include "yaush.h"

#define PIPE_SIZE 4096
static char pipebuf[PIPE_SIZE];

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
	
	int pfdout[2], pfdin[2];
	if (cmd->depend==1 && prevret != 0)
		return prevret; // AND
	if (cmd->depend==2 && prevret == 0)
		return prevret; // OR

	// special commands
	if (strcmp(cmd->cmd, "exit") == 0)
	{
		exit(0);
	}
	// preparation
	// pipe
	if (cmd->pp | 1)
	{	// write
		pipe(pfdout);
	}
	if (cmd->pp | 2)
	{	// read
		pipe(pfdin);
	}

	int pid=0, ret = 0, status = 0;
	pid = fork();
	if (pid == 0)
	{
		if (cmd->fout != NULL)
		{
			int fdout;
			if (cmd->append == 0)
				fdout = open(cmd->fout, O_CREAT|O_WRONLY|O_TRUNC, 0644);
			else
				fdout = open(cmd->fout, O_CREAT|O_WRONLY|O_APPEND, 0644);
			if (fdout != -1)
			{
				dup2(fdout, STDOUT_FILENO);
				close(fdout);
			}
			else
			{
				perror("Cannot open file\n");
				exit(2);
			}
		}
		else if (cmd->pp & 1)
		{
			dup2(pfdout[1], STDOUT_FILENO);
			close(pfdout[0]);
			close(pfdout[1]);
		}
		if (cmd->fin != NULL)
		{
			int fdin;
			fdin = open(cmd->fin, O_RDONLY, 0644);
			if (fdin != -1)
			{
				dup2(fdin, STDIN_FILENO);
				close(fdin);
			}
			else
			{
				perror("Cannot open file\n");
				exit(2);
			}
		}
		else if (cmd->pp & 2)
		{
			dup2(pfdin[0], STDIN_FILENO);
			close(pfdin[0]);
			close(pfdin[1]);
		}

		// Run command
		ret = execvp(cmd->cmd, arglist);
		if (ret ==-1)
		{
			printf("errno: %d\n",errno);
			exit(errno);
		}
		else
			exit(0);
	}
	else
	{
		// pipe input
		if (cmd->pp & 2)
		{
			close(pfdin[0]);
			write(pfdin[1], pipebuf, strlen(pipebuf));
			close(pfdin[1]);
		}
		pid = wait(&status);

		// pipe output
		if (cmd->pp & 1)
		{
			memset(pipebuf, 0, PIPE_SIZE);
			close(pfdout[1]);
			read(pfdout[0], pipebuf, PIPE_SIZE);
			close(pfdout[0]);
		}

		ret = WEXITSTATUS(status);
		return ret;
	}
}

int execute_cmds(CommandList * clist)
{
	//print_cmdlist(clist);
	Command * curcmd = clist->command->next;
	int ret = 0;
	while(curcmd!= NULL)
	{
		ret = execute_cmd(curcmd, ret);
		curcmd = curcmd->next;
	}
	return ret;
}
