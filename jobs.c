#include "yaush.h"

static Jobnode jobhead;
void job_init()
{
	jobhead.next =NULL;
}

int cmd_jobs(char * argv)
{
	return 0;

}


int cmd_bg(char * argv)
{
	return 0;
}

int cmd_fg(char * argv)
{
	return 0;
}
