#include "MyFunction.h"
#include "common.h"

static pid_t *childpid = NULL;
static int maxfd = 0;

#ifdef OPEN_MAX
static long openmax = OPEN_MAX;
#else
static long openmax = 0;
#endif

//static long openmax = 0;

MyFunction::MyFunction()
{
}


MyFunction::~MyFunction()
{
}

long open_max()
{
	if (openmax == 0)
	{
		errno = 0;
		
		if ((openmax = sysconf(_SC_OPEN_MAX)) < 0)
		{
			if (errno == 0)
			{
				openmax = OPEN_MAX_GUESS;
			}
			else
			{
				cerr << "sysconf error for _SC_OPEN_MAX" << endl;;
			}
		}
	}
	return (openmax);
}

FILE *popen(const char *cmdstring, const char * type)
{
	int i, pfd[2];
	pid_t pid;
	FILE *fp;
	
	if ((type[0] != 'r' && type[0] != 'w') || (type[1] != 0))
	{
		errno = EINVAL;
		return NULL;
	}
	
	if (childpid == NULL)
	{
		maxfd = open_max();
		if ((childpid = (pid_t *)calloc(maxfd, sizeof(pid_t))) == NULL)
		{
			return NULL;
		}
	}
	
	if (pipe(pfd) < 0)
	{
		cerr << "pipe failed, errno:%v" << errno << endl;
		return NULL;
	}
	
	if (pfd[0] >= maxfd || pfd[1] >= maxfd)
	{
		close(pfd[0]);
		close(pfd[1]);
		errno = EMFILE;
		return NULL;
	}
	
	if ((pid = fork()) < 0)
	{
		cerr << "fork error, errno:" << errno << endl;
		close(pfd[0]);
		close(pfd[1]);
		return NULL;
	}
	else if (pid == 0)
	{
		if (type[0] == 'r')
		{
			close(pfd[0]);
			if (pfd[1] != STDOUT_FILENO)
			{
				//将STDOUT_FILENO的值指定为pfd[1]
				dup2(pfd[1], STDOUT_FILENO);
				close(pfd[1]);
			}
		}
		else
		{
			close(pfd[1]);
			if (pfd[0] != STDIN_FILENO)
			{
				//将STDOUT_FILENO的值指定为pfd[1]
				dup2(pfd[0], STDIN_FILENO);
				close(pfd[0]);
			}
		}
		
		for (i = 0; i < maxfd; i++)
		{
			if (childpid[i] > 0)
			{
				close(i);
			}
		}
		
		execl("/bin/sh", "sh", "-c", cmdstring, 0);
		_exit(127);
	}
	else if (pid > 0)
	{
		if (type[0] == 'r')
		{
			close(pfd[1]);
			if ((fp = fdopen(pfd[0], type)) == NULL)
			{
				return NULL;
			}
		}
		else
		{
			close(pfd[0]);
			if ((fp = fdopen(pfd[1], type)) == NULL)
			{
				return NULL;
			}
		}
		
		childpid[fileno(fp)] = pid;
		return fp;
	}
	
	return NULL;
}

int pclose(FILE *fp)
{
	int fd, stat;
	pid_t pid;
	
	if (childpid == NULL)
	{// popen没有被调用过
		errno = EINVAL;
		return -1;
	}
	
	fd = fileno(fp);
	if ((long)fp >= (long)maxfd)
	{
		errno = EINVAL;
		return -1;
	}
	
	if ((pid = childpid[pid]) == 0)
	{
		errno = EINVAL;
		return -1;
	}
	
	childpid[fd] = 0;
	if (fclose(fp) == EOF)
	{
		return -1;
	}
	
	while (waitpid(pid, &stat, 0) < 0)
	{
		if (errno != EINTR)
		{
			return -1;
		}
	}
	return stat;
}
