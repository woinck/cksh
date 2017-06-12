#include "yaush.h"

static Jobnode jobhead;

Jobnode * find_job(int pid)
{
	Jobnode * curjob = &jobhead;
	while(curjob->next != NULL && curjob->next->pid != pid);
	if (curjob->next != NULL)
		return curjob->next;
	else
		return NULL;
}
void del_job(int pid)
{
	Jobnode * curjob = &jobhead;
	Jobnode * target;
	
	while(curjob->next != NULL && curjob->next->pid != pid);
	if (curjob->next != NULL)
	{
		target = curjob->next;
		curjob->next = curjob->next->next;
		free(target->cmd);
		free(target);
	}
}

void set_job_status(int pid, JobStatus status)
{
	Jobnode * curjob = find_job(pid);
	curjob->status = status;
}

void job_update()
{
	int wpid = 0;
	int status = 0;
	do
	{
		wpid = waitpid(0, &status, WNOHANG);
		if (wpid < 0) break;
		if (wpid > 0)
		{
			if (WIFEXITED(status))
			{
				del_job(wpid);
				//printf("Process %d exited\n", wpid);
			}
			if (WIFSTOPPED(status))
				set_job_status(wpid, stopped);
			if (WIFCONTINUED(status))
				set_job_status(wpid, running);
		}
	}
	while (wpid > 0);

}
void job_init()
{
	jobhead.next =NULL;
}

void add_job(int pid, char * line)
{
	Jobnode * curjob = &jobhead;
	while(curjob->next != NULL)
		curjob = curjob->next;
	curjob->next = malloc(sizeof(Jobnode));
	curjob = curjob->next;
	curjob->background = 1;
	curjob->pid = pid;
	curjob->cmd = strdup(line);
	curjob->status = running;
	curjob->next = NULL;
}

int cmd_jobs()
{
	int i = 0;
	Jobnode * curjob = &jobhead;
	while(curjob->next != NULL)
	{
		curjob = curjob->next;
		printf("[%d]\t%d\t\t%s\n", i, curjob->pid, curjob->cmd);
	}
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
