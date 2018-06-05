#pragma once
#include <fcntl.h>
#include <stdio.h>

class MyFunction
{
public:
	MyFunction();
	virtual ~MyFunction();
};


#define OPEN_MAX_GUESS 256

long open_max();

FILE *popen(const char *cmdstring, const char * type);
int pclose(FILE *fp);