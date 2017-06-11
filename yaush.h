#ifndef _YAUSH_H
#define _YAUSH_H

const char * yaush_prompt = "yaush> ";

typedef struct {
	char *name;
	rl_icpfunc_t * func;
	char * doc;
} COMMAND;


#endif
