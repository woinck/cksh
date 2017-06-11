#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "yaush.h"

static char * line_read = (char *) NULL;


int com_pwd PARAMS((char *));
//COMMAND commands[] = {
//	{ "cd", com_cd, "Change to directory DIR"},
//	{ "delete", com_delete, "Delete FILE" },
//	{ "help", com_help, "Synonym for 'help'"},
//	{ "ls", com_list, "List files"},
	//{ "pwd", com_pwd, "Print the current workdir"}
//};

char * commands[]={"pwd","cd"};

char * rl_gets()
{
	if(line_read)
	{
		free(line_read);
		line_read = (char *) NULL;
	}
	line_read = readline(yaush_prompt);

	if(line_read && *line_read)
		add_history(line_read);
	return(line_read);
}

static char * command_generator(const char *text, int state)
{
	const char * name;
	static int list_index, len;

	if (!state)
	{
		list_index = 0;
		len = strlen(text);
	}

	while ((name = commands[list_index]))
	{
		list_index++;
		if (strncmp (name, text, len) == 0)
			return strdup(name);
	}

	return ((char *) NULL);
}

char ** command_completion(const char * text, int start, int end)
{
	char ** matches = NULL;
	if (start == 0)
	{
		matches = rl_completion_matches(text, command_generator);
	}
	return (matches);
}

void initialize_readline()
{
	rl_readline_name = "yaush> ";
	rl_attempted_completion_function = command_completion;
	return;
}

char * stripwhite(char *str)
{
	register char *s, *t;
	for (s=str;whitespace(*s); s++)
		;
	if (*s == 0)
		return(s);
	t = s + strlen(s)-1;
	while (t > s && whitespace (*t))
		t--;
	++t;
	*t = '\0';
	return s;
}

void execute_line(char * line)
{
	printf("%s\n", line);
	return;
}

int main(int argc, char **argv)
{
	initialize_readline(); // Bind the command completer
	char * line, * s;;

	// Loop reading and exec
	for(;;)
	{
		line = rl_gets();
		s = stripwhite(line);
		execute_line(line);
	}
}

