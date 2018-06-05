#pragma once
#include <iostream>
#include <signal.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <sys/wait.h>
#include <map>
#include <setjmp.h>
#include <stdlib.h> 
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <fcntl.h>
#include <time.h> 
#include <string.h>
#include <sys/shm.h>
#include <cstdio>
#define MAXLINE 1024
#define SHM_SIZE 1024*1204

typedef void Sigfunc(int);
typedef void Sigfunc2(int, siginfo_t *, void *);
using namespace std;

extern map<int, string> threadmap;