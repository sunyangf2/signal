#include "process.h"
#include "common.h"


process::process()
{
}


process::~process()
{
}

//实现system函数
int mysystem(const char *cmdstring)
{
	pid_t pid;
	int status;
	if (cmdstring == NULL)
	{
		return 1;
	}
	
	if ((pid = fork()) < 0)
	{
		status = -1;
	}
	else if (pid == 0)
	{
		//child
		if(execl("/bin/sh", "sh", "-c", cmdstring, 0) == -1)
		//if(execl(cmdstring, "execobj", 0) == -1)
		{
			cerr << "exec failed, errno:" << errno << endl;
			exit(127);
		}
	
	}
	else if (pid > 0)
	{
		//parent
		while(waitpid(pid, &status, 0) < 0)
		{
			if (errno != EINTR)
			{
				status = -1;
				break;
			}
		}
	}
	return status;
}

int testsystem()
{
	int status;
	if (mysystem("date") < 0)
	{
		cerr << "system faild." << endl;
		return -1;
	}
	
	if (mysystem("/tmp/VisualGDB/c/workspace/signal/signal/Debug/execobj") < 0)
	{
		cerr << "system faild." <<status<< endl;
		return -1;
	}
	return 0;
}

int testvfork()
{
	pid_t pid;
	pid = vfork();
	if (pid == 0)
	{
		//		if(pause() == -1)
		//		{
		//			if (errno == EINTR)
		//			{
		//				cout << "received a signal in child process" << endl;
		//			}
		//		}
		cout<<"child process is running."<<endl;
		execl("/tmp/VisualGDB/c/workspace/signal/signal/Debug/execobj", "execobj", 0);
	}
	else 
	{
		int status = 0;
		wait(&status);
		cout << "parent process is awake." << endl;;
	}
	return 0;
}

void threadcleanup(void *para)
{
	char * output = (char*)para;
	cout << output << endl;
}

void* threadfunc(void *para)
{
	printf("thread 1 start\n");
	pthread_cleanup_push(threadcleanup, (void *)"first register...");
	pthread_cleanup_push(threadcleanup, (void *)"second register...");

	printf("thread 1 push comlete\n");
	return (void*)"hahahah";

	pthread_cleanup_pop(0);
	pthread_cleanup_pop(1);
	pthread_exit((void*)"hahahah");
	//return (void*)"hahahah";
	
	
	
    
	return (void*)"xixix";
	//pthread_exit((void*)"xixixixix");
}

void exitfunc1()
{
	cout << "call exitfunc1()" << endl;
}

void exitfunc2()
{
	cout << "call exitfunc2()" << endl;
}


int testthreadjoin()
{
	pthread_t pid;
	atexit(exitfunc1);
	int errcode = pthread_create(&pid, NULL, threadfunc, NULL);
	if (errcode != 0)
	{
		cerr << "pthread_create failed, errno:" << errno << endl;
		return -1;
	}
	atexit(exitfunc2);
	void* ret = NULL;
	/*errcode = pthread_detach(pid);
	if (errcode != 0)
	{
		cerr << "pthread_detach detached failed, errcode:" << errcode << endl;
		return 0;
	}*/
	errcode = pthread_join(pid, &ret);
	if (errcode != 0)
	{
		cerr << "pthread_join failed, errno:" << errcode << endl;
		return -1;
	}
	cout << "return of the thread is :" << (char*)ret << endl;
	return 0;
}
int processId = 1; 
void* multiforkfunc(void * para)
{
	static int count = 0;
	string threadId = threadmap[pthread_self()];
	for (;;)
	{
		if (threadId == "p3" && count == 3)
		{
			pid_t pid;
			pid = fork();
			if (pid < 0)
			{
				cerr << "fork failed, errno:" << errno << endl;
				return (void*)-1;
			}
			else if (pid == 0)
			{
				processId++;
				cout << "child process is running, process id:" << processId << endl;
			}
			else
			{

			}
		}
		cout << "proess[" << processId << "]::thread[" << threadId << "] is alive..." << endl;
		sleep(2);
		if (threadId == "p3") count++;
	}
	return (void *)0;
}

int testmultithreadfork()
{
	pthread_t p1, p2, p3, p4;
	cout << "main thread is:" << pthread_self() << endl;
	threadmap.insert(pair<int, string>(pthread_self(), "main thread"));
	
	int errcode = 0;
	errcode = pthread_create(&p1, NULL, multiforkfunc, NULL);
	if (errcode != 0)
	{
		cerr << "create thread failed, errcode:" << errcode << endl;
		return -1;
	}
	threadmap.insert(pair<int, string>(p1, "p1"));
	
	errcode = pthread_create(&p2, NULL, multiforkfunc, NULL);
	if (errcode != 0)
	{
		cerr << "create thread failed, errcode:" << errcode << endl;
		return -1;
	}
	threadmap.insert(pair<int, string>(p2, "p2"));
	errcode = pthread_create(&p3, NULL, multiforkfunc, NULL);
	if (errcode != 0)
	{
		cerr << "create thread failed, errcode:" << errcode << endl;
		return -1;
	}
	threadmap.insert(pair<int, string>(p3, "p3"));
	errcode = pthread_create(&p4, NULL, multiforkfunc, NULL);
	if (errcode != 0)
	{
		cerr << "create thread failed, errcode:" << errcode << endl;
		return -1;
	}
	threadmap.insert(pair<int, string>(p4, "p4"));

	pause();
	return 0; 
}

void * threadcancel(void * para)
{
	//	int errcode = pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
	//	if (errcode != 0)
	//	{
	//		cerr << "pthread_setcancelstate failed, errcode:" << errcode << endl;
	//		return (void *)2;
	//	}
	
	int errcode = pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
	if (errcode != 0)
	{
		cerr << "pthread_setcanceltype failed, errcode:" << errcode << endl;
		return (void*) 3;
	}
	cout << "ready to cancel..." << endl;
	//pause();
	sleep(3);
	cout << "after cancel..." << endl;
	return (void*)1;
}

int testthreadcancel()
{
	pthread_t id, id2;
	
	int errcode = pthread_create(&id, NULL, threadcancel, NULL);
	if (errcode != 0)
	{
		cerr << "pthread_create failed, errcode:" << errcode << endl;
		return -1;
	}

	//sleep(1);
	errcode = pthread_cancel(id);
	if (errcode != 0)
	{
		cerr << "thread_cancel failed, errcode:" << errcode << endl;
		return -1;
	}
	
    void* ret;
	
	cout << "before pthread_join" << endl;
	errcode = pthread_join(id, &ret);
	if (errcode != 0)
	{
		cerr << "pthread_join failed, errcode:"<<errcode << endl;
		return -1;
	}
	cout << "return from thread:" << ret << endl;
	return 0;
}

int testthreadattr()
{
	pthread_t id;
	pthread_attr_t pattr;
	
	int errcode = pthread_attr_init(&pattr);
	if(errcode != 0)
	{
		cerr << "pthread_attr_init failed, errcode:" << errcode << endl ;
		return - 1 ;
	}
	
	int detachstate ;
	errcode = pthread_attr_getdetachstate(&pattr, &detachstate);
	if (errcode != 0)
	{
		cerr << "pthread_attr_getdetachstate failed, errcode:" << errcode << endl;
		return -1;
	}
	cout << "old detachstate:" << detachstate << endl;
	
	detachstate = 1;
	errcode = pthread_attr_setdetachstate(&pattr, detachstate);
	if (errcode != 0)
	{
		cerr << "pthread_attr_setdetachstate failed, errcode:" << errcode << endl;
		return -1;
	}
	errcode = pthread_attr_getdetachstate(&pattr, &detachstate);
	if (errcode != 0)
	{
		cerr << "pthread_attr_getdetachstate failed, errcode:" << errcode << endl;
		return -1;
	}
	cout << "new detachstate:" << detachstate << endl;
	
	void* stackend;
	size_t stacksize = 0;
	
	errcode = pthread_attr_getstacksize(&pattr, &stacksize);
	if (errcode != 0)
	{
		cerr << "pthread_attr_getstacksize failed, errcode:" << errcode << endl;
		return -1;
	}
	
	cout << "old stack size:" << stacksize << endl;
	stacksize += 1024;
	errcode = pthread_attr_setstacksize(&pattr, stacksize);
	if (errcode != 0)
	{
		cerr << "pthread_attr_getstacksize failed, errcode:" << errcode << endl;
		return -1;
	}
	
	cout << "new stack size:" << stacksize << endl;
	
	errcode = pthread_attr_getstack(&pattr, &stackend, &stacksize);
	if (errcode != 0)
	{
		cerr << "pthread_attr_getstack failed, errcode:"<<errcode << endl;
		return -1;
	}
	cout << "old stack end:" << stackend <<" stack length:"<<stacksize<< endl;
	
	size_t guardsize;
	errcode = pthread_attr_getguardsize(&pattr, &guardsize);
	if (errcode != 0)
	{
		cerr << "pthread_attr_getguardsize failed, errcode:" << errcode << endl;
		return -1;
	}
	
	cout << "old guardsize:" << guardsize << endl;
	guardsize = 0;
	
	errcode = pthread_attr_setguardsize(&pattr, guardsize);
	if (errcode != 0)
	{
		cerr << "pthread_attr_getguardsize failed, errcode:" << errcode << endl;
		return -1;
	}
	errcode = pthread_attr_getguardsize(&pattr, &guardsize);
	if (errcode != 0)
	{
		cerr << "pthread_attr_getguardsize failed, errcode:" << errcode << endl;
		return -1;
	}
	
	cout << "new guardsize:" << guardsize << endl;
	
	
	errcode = pthread_create(&id, &pattr, threadcancel, NULL);
	if (errcode != 0)
	{
		cerr << "pthread_create failed, errcode:" << errcode << endl;
		return -1;
	}
	
	errcode = pthread_attr_destroy(&pattr);
	if (errcode != 0)
	{
		cerr << "pthread_attr_destory failed, errcode:" << errcode << endl;
		return -1;
	}
	
	errcode = pthread_join(id,NULL);
	if (errcode != 0)
	{
		cerr << "pthread_join failed, errcode:" << errcode << endl;
		return -1;
	}
	return 0;
}

void prepare1()
{
	cout << "in prepare1...." << endl;
}

void parent1()
{
	cout << "in parent1..." << endl;
}

void child1()
{
	cout << "in child1..." << endl;
}

void prepare2()
{
	cout << "in prepare2...." << endl;
}

void parent2()
{
	cout << "in parent2..." << endl;
}

void child2()
{
	cout << "in child2..." << endl;
}


int testatfork()
{
	int errcode;
	errcode = pthread_atfork(prepare1, parent1, child1);
	if (errcode != 0)
	{
		cerr << "pthread_atfork failed, errcode:" << errcode << endl;
		return -1;
	}
	
	errcode = pthread_atfork(prepare2, parent2, child2);
	if (errcode != 0)
	{
		cerr << "pthread_atfork failed, errcode:" << errcode << endl;
		return -1;
	}
	pid_t pid;
	pid = fork();
	if (pid < 0)
	{
		cerr << "fork failed, errno:" << errno << endl;
		return -1;
	}
	else if (pid == 0)
	{
		cout << "child process is running..." << endl;
	}
	else
	{
		cout << "parent process is running..." << endl;
		int status;
		wait(&status);
	}
}

pthread_key_t thread_log_key;

void* privatekeyfunc(void* para)
{
	long i = (long)para;
	long *v = new long();
	*v = i;
	pthread_setspecific(thread_log_key, (void *)v);
	v = NULL;
	sleep(1);
	v = (long*)pthread_getspecific(thread_log_key);
	cout << "p[" << i << "] private value is:" << *v << endl;
	return (void *)1;
}

void clearprivatekey(void * v)
{
	cout << "clear p[" << *(long*)v << "]..." << endl;
	delete v;
}

int testthreadprivatekey()
{
	int i;
	pthread_t threads[5];
	int errcode;
	
	pthread_key_create(&thread_log_key, clearprivatekey);
	
	for (i = 0; i < 5; i++)
	{
		errcode = pthread_create(&threads[i], NULL, privatekeyfunc, (void *)i);
		if (errcode != 0)
		{
			cerr << "pthread_create failed, errcode:" << errcode << endl;
			return -1;
		}
	}
	
	for (i = 0; i < 5; i++)
	{
		pthread_join(threads[i], NULL);
	}
	cout << "about to quit ..." << endl;
	return 0;
}

pthread_once_t initflag;

void oncefunc()
{
	cout << "int oncefunc..." << endl;
}

void* threadoncefunc(void *para)
{
	long i = (long)para;
	cout << "in process[" << i << "] ..." << endl;
	if (pthread_once(&initflag, oncefunc) != 0)
	{
		cerr << "pthread_once failed" << endl;
	}
	
	return (void*)1;
}

int initthreads(int threadnum, void* (*fname)(void*))
{
	int i;
	pthread_t* threads = new pthread_t[threadnum];
	int errcode;
	
	for (i = 0; i < threadnum; i++)
	{
		errcode = pthread_create(&threads[i], NULL, fname, (void *)i);
		if (errcode != 0)
		{
			cerr << "pthread_create failed, errcode:" << errcode << endl;
			return -1;
		}
	}
	
	for (i = 0; i < threadnum; i++)
	{
		pthread_join(threads[i], NULL);
	}
	delete[] threads;
	cout << "about to quit ..." << endl;
	return 0;
}

int testpthreadonce() //threadoncefunc
{
	return initthreads(5, threadoncefunc);
}


pthread_mutex_t* lock;
void* mutexthread(void* para)
{
	pid_t id = getpid();
	int threadno = (long)para;
	int errcode = 0;
	timespec t;
	for (;;)
	{
		int x = rand() % 10;
		int y = rand() % 10;
		int z = rand() % 10;
		t.tv_nsec = x * 100000000 + y * 10000000 + z * 1000000;
		t.tv_sec = 0;
		errcode = pthread_mutex_lock(lock);
		//errcode = pthread_mutex_trylock(lock);
		//errcode = pthread_mutex_timedlock(lock, &t);
		if (errcode != 0 && errcode != EBUSY &&errcode != ETIMEDOUT)
		{
			cerr << "process["<<id<<"] pthread_mutex_lock failed, errcode:" << errcode << endl;
			//return (void*)2;
			break;
		}
		else if (errcode == EBUSY)
		{
			//sleep(1);
			continue;
		}else if(errcode == ETIMEDOUT)
		{
			continue;
		}
	
		cout << "process[" << id << "] thread p" << threadno << " is running ..." << endl;
		//sleep(1);
		//if(threadno != 3)
		if(id % 2 == 1)
		{
			cout << "process[" << id << "] thread p" << threadno << " hold the lock" << endl;
			sleep(1);
			continue;//break;
		}
		errcode = pthread_mutex_unlock(lock);
		if (errcode != 0)
		{
			cerr << "pthread_mutex_unlock failed, errcode:" << errcode << endl;
			return (void*)2;
		}
		cout << "process[" << id << "] thread p" << threadno << " release the lock" << endl;
		sleep(1);
	}

	cout << "process[" << id << "] thread p" << threadno << " quit ..." << endl;
	return (void*)1;
}

int testmutext()
{
	cout << "EINVAL:" << EINVAL<<" EDEADLK:" << EDEADLK << endl;
	
	lock = new pthread_mutex_t;
	pthread_mutexattr_t muattr;
	int errcode = pthread_mutexattr_init(&muattr);
	if (errcode != 0)
	{
		cerr << "init mutex attrbute failed, errcode:" << errcode << endl;
		return -1;
	}
	
	int shared;
	errcode = pthread_mutexattr_getpshared(&muattr, &shared);
	if (errcode != 0)
	{
		cerr << "pthread_attr_getshared failed, errcode:" << errcode << endl;
		return -1;
	}
	cout << "old shared:" << shared << " PTHREAD_PROCESS_PRIVATE:" << PTHREAD_PROCESS_PRIVATE << " PTHREAD_PROCESS_SHARED:" << PTHREAD_PROCESS_SHARED << endl;
	
	shared = PTHREAD_PROCESS_SHARED;
	errcode = pthread_mutexattr_setpshared(&muattr, shared);
	if (errcode != 0)
	{
		cerr << "pthread_attr_setdetachstate failed, errcode:" << errcode << endl;
		return -1;
	}
	errcode = pthread_mutexattr_getpshared(&muattr, &shared);
	if (errcode != 0)
	{
		cerr << "pthread_mutexattr_getpshared failed, errcode:" << errcode << endl;
		return -1;
	}
	
	cout << "new shared:" << shared << " PTHREAD_PROCESS_PRIVATE:" << PTHREAD_PROCESS_PRIVATE << " PTHREAD_PROCESS_SHARED:" << PTHREAD_PROCESS_SHARED << endl;
	
	int mutextype = PTHREAD_MUTEX_RECURSIVE;
	errcode = pthread_mutexattr_settype(&muattr, mutextype);
	if (errcode != 0)
	{
		cerr << "pthread_mutexattr_settype failed, errcode:" << errcode << endl;
		return -1;
	}
	
	fork();
	
	errcode = pthread_mutex_init(lock, &muattr);
	if (errcode != 0)
	{
		cerr << "init mutex failed. errcode:" << errcode << endl;
		return -1;
	}
		
	initthreads(5, mutexthread);
	
	errcode = pthread_mutexattr_destroy(&muattr);
	if (errcode != 0)
	{
		cerr << "destroy mutex attrbute failed, errcode:" << errcode << endl;
		return -1;
	}
	
	errcode = pthread_mutex_destroy(lock);
	if (errcode != NULL)
	{
		cerr << "destory mutex failed, errcode:" << errcode << endl;
		return -1;
	}
	
	return 0;
}


int testshmmtext()
{
	int shmid;
	void *shmptr;
	shmid = shmget(IPC_PRIVATE, SHM_SIZE, 0600 | IPC_CREAT);
	if (shmid == -1)
	{
		cerr << "shmget failed, errno:" << errno << endl;
		return -1;
	}
	
	shmptr = shmat(shmid, 0, 0);
	if (shmptr == (void*) - 1)
	{
		cerr << "shmat failed, errno:" << errno << endl;
		if (shmctl(shmid, IPC_RMID, 0) < 0)
		{
			cerr << "delete shm failed, errno:" << errno << endl;
			return -1;
		}
		return -1;
	}
	
	pthread_mutexattr_t muattr ;
	lock = (pthread_mutex_t*)shmptr;
	//lock = new pthread_mutex_t;
	
	int shared = PTHREAD_PROCESS_SHARED;
	int errcode = pthread_mutexattr_setpshared(&muattr, shared);
	if (errcode != 0)
	{
		cerr << "pthread_attr_setdetachstate failed, errcode:" << errcode << endl;
		return -1;
	}
	
	errcode = pthread_mutex_init(lock, &muattr);
	if (errcode != 0)
	{
		cerr << "init mutex failed. errcode:" << errcode << endl;
		return -1;
	}
	
	fork();
	
	//sleep(1);
	//当把pthread_mutex_init位置放在fork后面的时候, 发现PTHREAD_PROCESS_SHARED好像在一根进程中并没有生效
	//原因可能是父子进程调用同一个mutex的pthread_mutex_init产生了竞态, 导致第一个调用pthread_mutex_init进程的PTHREAD_PROCESS_SHARED没有生效
	//说明pthread_mutex_init不是线程安全的函数
	initthreads(5, mutexthread);
	
	errcode = pthread_mutexattr_destroy(&muattr);
	if (errcode != 0)
	{
		cerr << "destroy mutex attrbute failed, errcode:" << errcode << endl;
		return -1;
	}
	
	errcode = pthread_mutex_destroy(lock);
	if (errcode != NULL)
	{
		cerr << "destory mutex failed, errcode:" << errcode << endl;
		return -1;
	}
	
	if (shmdt(shmptr) < 0)
	{
		cerr << "shmdt failed, errno:" << errno << endl;
		return -1;
	}
	
	if (shmctl(shmid, IPC_RMID, 0) < 0)
	{
		cerr << "delete shm failed, errno:" << errno << endl;
		return -1;
	}
	return 0;
}

int testrobustmutex()
{
	int shmid;
	void *shmptr;
	shmid = shmget(IPC_PRIVATE, SHM_SIZE, 0600 | IPC_CREAT);
	if (shmid == -1)
	{
		cerr << "shmget failed, errno:" << errno << endl;
		return -1;
	}
	
	shmptr = shmat(shmid, 0, 0);
	if (shmptr == (void*) - 1)
	{
		cerr << "shmat failed, errno:" << errno << endl;
		if (shmctl(shmid, IPC_RMID, 0) < 0)
		{
			cerr << "delete shm failed, errno:" << errno << endl;
			return -1;
		}
		return -1;
	}
	
	shmid_ds ds;
	if (shmctl(shmid, IPC_STAT, &ds) < 0)
	{
		cerr << "delete shm failed in child process, errno:" << errno << endl;
		exit(-2); //return -2;
	}
	
	cout << "nattach count: " << ds.shm_nattch << endl;
	pthread_mutexattr_t muattr;
	lock = (pthread_mutex_t*)shmptr;
	
	int shared = PTHREAD_PROCESS_SHARED;
	int errcode = pthread_mutexattr_setpshared(&muattr, shared);
	if (errcode != 0)
	{
		cerr << "pthread_attr_setdetachstate failed, errcode:" << errcode << endl;
		return -1;
	}
	
	errcode = pthread_mutexattr_setrobust(&muattr, PTHREAD_MUTEX_ROBUST);
	if (errcode != 0)
	{
		cerr << "pthread_mutexattr_setrobust failed, errcode:" << errcode << endl;
		return -1;
	}
	
	errcode = pthread_mutex_init(lock, &muattr);
	if (errcode != 0)
	{
		cerr << "init mutex failed. errcode:" << errcode << endl;
		return -1;
	}
	
	pid_t pid = fork();
	if (pid == 0)
	{
		pthread_mutex_lock(lock);
		/*if (shmdt(shmptr) == -1)
		{
			cerr << "shmdt failed in child process, errno:" << errno << endl;
			exit(-1);
		}
		
		if (shmctl(shmid, IPC_RMID, 0) < 0)
		{
			cerr << "delete shm failed in child process, errno:" << errno << endl;
			exit(-2);//return -2;
		}*/
		
		if (shmctl(shmid, IPC_STAT, &ds) < 0)
		{
			cerr << "delete shm failed in child process, errno:" << errno << endl;
			exit(-2);  //return -2;
		}
	
		cout << "nattach count in child process: " << ds.shm_nattch << endl;
		exit(0);
	}
	
	int state = 0;
	if (waitpid(pid, &state, 0) == -1)
	{
		cerr << "waitpid failed, errno:"<<errno << endl;
		return 0; 
	}
	
	if (shmctl(shmid, IPC_STAT, &ds) < 0)
	{
		cerr << "delete shm failed in parent process, errno:" << errno << endl;
		exit(-2);   //return -2;
	}
	
	cout << "nattach count in parent process: " << ds.shm_nattch << endl;
	
	cout << "parent process is awake..." << endl;
	errcode = pthread_mutex_lock(lock);
	if (errcode != 0 && errcode == EOWNERDEAD)
	{
		cout << "mutex should be set to consistent..." << endl;
		if ( (errcode = pthread_mutex_consistent(lock)) != 0 )
		{
			cerr << "pthread_mutex_consistentt failed, errcode:"<<errcode << endl;
			return -1;
		}
		
		errcode = pthread_mutex_unlock(lock);
		if (errcode != 0)
		{
			cerr << "pthread_mutex_unlock errcode:" << errcode << endl;
			return -1;
		}
		errcode = pthread_mutex_lock(lock);
		if (errcode != 0)
		{
			cerr << "pthread_mutex_unlock errcode:" << errcode << endl;
			return -1;
		}
		cout << "make mutext become consistent again..." << endl;
	}
	else if (errcode != 0){
		cout << "pthread_mutex_lock errcode:" << errcode << " EOWNERDEAD:" << EOWNERDEAD << endl;
	}
	cout << "has got the lock ..." << endl;
	errcode = pthread_mutex_unlock(lock);
	if (errcode != 0)
	{
		cout << "pthread_mutex_unlock errcode:" << errcode << endl;
	}
	
	if (shmdt(shmptr) == -1)
	{
		cerr << "shmdt failed in parent process, errno:" << errno << endl;
		return -1;
	}
		
	if (shmctl(shmid, IPC_RMID, 0) < 0)
	{
		cerr << "delete shm failed in parent process, errno:" << errno << endl;
		return -2;
	}
	return 0;
}

int testshmlifespan(int &shmid, void *&shmptr)
{
	key_t IPC_SHARED = ftok("/tmp/VisualGDB/c/workspace/signal/signal", 1);
	cout << "ipc key is:" << IPC_SHARED << endl;
	int errcode;
	
	shmid = shmget(IPC_SHARED, SHM_SIZE, 0600 | IPC_CREAT | IPC_EXCL);
	bool first = true;
	if (shmid == -1 && errno == EEXIST)
	{
		cout << "shm already exist..." << endl;
		shmid = shmget(IPC_SHARED, SHM_SIZE, 0600);
		first = false;
	}
	
	if (shmid == -1)
	{
		cerr << "shmget failed, errno:" << errno << endl;
		return -1;
	}
	
	shmptr = shmat(shmid, 0, 0);
	if (shmptr == (void*) - 1)
	{
		cerr << "shmat failed, errno:" << errno << endl;
		if (shmctl(shmid, IPC_RMID, 0) < 0)
		{
			cerr << "delete shm failed, errno:" << errno << endl;
			return -1;
		}
		return -1;
	}
	
	int* v = (int*)shmptr;
	lock = (pthread_mutex_t*)((char *)shmptr + 2*sizeof(int));
	if (first)
	{
		*v = 0;
		int shared = PTHREAD_PROCESS_SHARED;
		pthread_mutexattr_t muattr;
		errcode = pthread_mutexattr_setpshared(&muattr, shared);
		if (errcode != 0)
		{
			cerr << "pthread_attr_setdetachstate failed, errcode:" << errcode << endl;
			return -1;
		}
	
		errcode = pthread_mutexattr_setrobust(&muattr, PTHREAD_MUTEX_ROBUST);
		if (errcode != 0)
		{
			cerr << "pthread_mutexattr_setrobust failed, errcode:" << errcode << endl;
			return -1;
		}
	
		errcode = pthread_mutex_init(lock, &muattr);
		if (errcode != 0)
		{
			cerr << "init mutex failed. errcode:" << errcode << endl;
			return -1;
		}
		errcode = pthread_mutex_lock(lock);
		if (errcode != 0)
		{
			cerr << "pthread_mutex_unlock errcode:" << errcode << endl;
			return -1;
		}
	}
	else
	{
		(*v)++;
		cout << "the shared v:" << *v << endl;
		errcode = pthread_mutex_lock(lock);
		if (errcode != 0 && errcode == EOWNERDEAD)
		{
			cout << "mutex should be set to consistent..." << endl;
			if ((errcode = pthread_mutex_consistent(lock)) != 0)
			{
				cerr << "pthread_mutex_consistentt failed, errcode:" << errcode << endl;
				return -1;
			}
		
			errcode = pthread_mutex_unlock(lock);
			if (errcode != 0)
			{
				cerr << "pthread_mutex_unlock errcode:" << errcode << endl;
				return -1;
			}
			errcode = pthread_mutex_lock(lock);
			if (errcode != 0)
			{
				cerr << "pthread_mutex_unlock errcode:" << errcode << endl;
				return -1;
			}
			cout << "make mutext become consistent again..." << endl;
		}
	}
	
	return 0; 
}

int testshmlifespan0()
{
	int shmid;
	void *shmptr;
	testshmlifespan(shmid, shmptr);
	//pause();
	if (shmdt(shmptr) < 0)
	{
		cerr << "shmdt failed, errno:" << errno << endl;
		return -1;
	}
	
	if (shmctl(shmid, IPC_RMID, 0) < 0)
	{
		cerr << "delete shm failed, errno:" << errno << endl;
		return -1;
	}
	return 0; 
}

int testshmlifespan1()
{
	int shmid;
	void *shmptr;
	testshmlifespan(shmid, shmptr);
	if (shmdt(shmptr) < 0)
	{
		cerr << "shmdt failed, errno:" << errno << endl;
		return -1;
	}
	
	/*if (shmctl(shmid, IPC_RMID, 0) < 0)
	{
		cerr << "delete shm failed, errno:" << errno << endl;
		return -1;
	}*/
	return 0; 
}

pthread_cond_t* cond;
void * condthread(void *para)
{
	int* pval = (int*)para;
	int errcode;
	for (int i = 0; i < 10; i++)
	{
		errcode = pthread_mutex_lock(lock);
		if (errcode != 0)
		{
			cerr << "lock failed, errcode:" << errcode << endl;
		}
		else
		{
			(*pval)++;
		}
		pthread_mutex_unlock(lock);
		pthread_cond_signal(cond);
		sleep(1);
	}
	return NULL;
}

int testconv()
{
	pthread_t id;
	int errcode;
	int value = 0;
	lock = new pthread_mutex_t;
	cond = new pthread_cond_t;
	
	errcode = pthread_mutex_init(lock, NULL);
	if (errcode != 0)
	{
		cerr << "mutex init failed, errcode:" << errcode << endl;
		return -1;
	}
	
	errcode = pthread_cond_init(cond, NULL);
	if (errcode != 0)
	{
		cerr << "cond init failed, errcode:" << errcode << endl;
		return -1;
	}
	
	errcode = pthread_mutex_lock(lock);
	if (errcode != 0)
	{
		cerr << "pthread_mutex_lock failed, errcode:" << errcode << endl;
		return -1; 
	}
	
	errcode = pthread_create(&id, NULL, condthread, &value);
	if (errcode != 0)
	{
		cerr << "create thread failed, errcode:" << errcode << endl;
		return -1;
	}

	for (int i = 0; i < 10; i++)
	{
		errcode = pthread_cond_wait(cond, lock);
		if (errcode != 0)
		{
			cerr << "pthread_cond_wait failed, errcode:"<<errcode << endl;
			break;
		}
		
		cout << "value:"<<value << endl;;
	}
	return 0;
}

pthread_barrier_t barrier;

void* barrierfunc(void *para)
{
	cout << "in barrier func" << endl;
	int errcode = pthread_barrier_wait(&barrier);
	if (errcode != 0 && errcode != PTHREAD_BARRIER_SERIAL_THREAD)
	{
		cerr << "barrier_wait2 failed, errcode:" << errcode << endl;
		return (void*)-1 ;
	}
	cout << "barrier func start after wait 1, with ret:"<<errcode << endl;
	
	errcode = pthread_barrier_wait(&barrier);
	if (errcode != 0 && errcode != PTHREAD_BARRIER_SERIAL_THREAD)
	{
		cerr << "barrier_wait2 failed, errcode:" << errcode << endl;
		return (void*) - 1 ;
	}
	cout << "barrier func start after wait 2,  with ret:" << errcode << endl;
	return NULL;
}

int testbarrier()
{
	pthread_t id;
	int errcode;
	
	errcode = pthread_barrier_init(&barrier, NULL, 2);
	if (errcode != 0 )
	{
		cerr << "pthread_barrier_init failed, errcode:" << errcode << endl;
		return -1;
	}
	
	errcode = pthread_create(&id, NULL, barrierfunc, NULL);
	if (errcode != 0)
	{
		cerr << "create thread failed, errcode:" << errcode << endl;
		return -1;
	}
	
	cout << "the thread has been started..." << endl;
	
	errcode = pthread_barrier_wait(&barrier);
	if (errcode != 0&& errcode != PTHREAD_BARRIER_SERIAL_THREAD)
	{
		cerr << "pthread_barrier_wait1 failed, errcode:" << errcode << endl;
		return - 1 ;
	}
	
	cout << "main func after wait1, with ret:" << errcode << endl;
	
	errcode = pthread_barrier_wait(&barrier);
	if (errcode != 0&& errcode != PTHREAD_BARRIER_SERIAL_THREAD)
	{
		cerr << "pthread_barrier_wait1 failed, errcode:" << errcode << endl;
		return -1 ;
	}
	
	cout << "main func after wait2, with ret:" << errcode << endl;
	return 0;
}

int testpipe()
{
	int fds[2];
	char buff[MAXLINE] = {0};
	int pid;
	if (pipe(fds) == -1)
	{
		cerr << "open pipe failed, errno:" <<errno<< endl;
		return -1;
	}
	
	pid = fork();
	if (pid == 0)
	{
		close(fds[1]);
		
		while (strcmp(buff, "bye") != 0)
		{
			int n = read(fds[0], buff, sizeof(buff));
			if (n < 0 && errno != EINTR)
			{
				cerr << "read failed, err:" << errno << endl;
				break;
			}
			else if (n == 0)
			{
				break;
			}
			else if (n > 0)
			{
				cout << "child process received:" << buff << endl;
				memset(buff, 0, sizeof(buff));
			}
		}
		close(fds[0]);
		exit(0);
	}
	
	close(fds[0]);
	
	scanf("%s", buff);
	
	while (strcmp(buff, "bye") != 0)
	{
		int n = 0;
		if (strlen(buff) > 0)
		{
			n = write(fds[1], buff, strlen(buff));
			if (n < 0 && errno != EINTR)
			{
				cerr << "write failed, err:" << errno << endl;
				break;
			}
			else if (n > 0)
			{
				memset(buff, 0, sizeof(buff));
				scanf("%s", buff);
			}
		}
	}
	
	close(fds[0]);
	
	return 0;
}


int testpopen()
{
	FILE *p = popen("date", "r");
	char buff[MAXLINE] = { 0 };
	fgets(buff, MAXLINE, p);
	cout << "pipe:" << buff ;
	pclose(p);
}

int testfifo()
{
	int fd;
	char buff[MAXLINE] = { 0 };
	/*if (mkfifo("/tmp/VisualGDB/c/workspace/signal/signal/Debug/myfifo", 0777) <0)
    {
	    cerr << "mkfifo failed, errno:"<<errno << endl;
	    return -1;
    }*/
	
	fd = open("/tmp/VisualGDB/c/workspace/signal/signal/Debug/myfifo", O_RDWR);
	if (fd == -1)
	{
		cerr << "open fifo faield, errno:" << errno << endl;
		return -1;
	}
	
	pid_t pid;
	int n;
	pid = fork();
	if (pid == 0)
	{
		n = read(fd, buff, MAXLINE);
		if (n > 0)
		{
			cout << "child process:" << buff << endl;
		}
		close(fd);
		cout << "child exit..." << endl;
		exit(0);
	}
	else if(pid > 0)
	{
		int stat;
		cout << "parent before send" << endl;
		n = write(fd, "send msg to child process", strlen("send msg to child process"));
		cout << "parent after send" << endl;
		if (n < 0)
		{
			cerr << "write failed, errno:" << errno << endl;
		}
		wait(&stat);
		close(fd);
	}
	cout << "quit testfifo" << endl;
	return 0;
}

typedef struct msg
{
	long mtype;
	char mtext[MAXLINE];
}Msg;

#define MY_MSG_TYPE  0x1

int testmsgqueue()
{
	key_t IPC_SHARED = ftok("/tmp/VisualGDB/c/workspace/signal/signal", 1);
	cout << "ipc key is:" << IPC_SHARED << endl;
	Msg message;
	//char * buff[MAXLINE] = { 0};
	
	int msqid;
	pid_t pid;
	msqid = msgget(IPC_SHARED, 0600 | IPC_CREAT | IPC_EXCL);
	if (msqid == -1 && errno == EEXIST)
	{
		cerr << "msg queue already exist..." << endl;
	}
	pid = fork();
	if (pid == 0)
	{
		cout << "in child process..." << endl;
		msqid = msgget(IPC_SHARED, 0600 );
		if (msqid == -1)
		{
			cerr << "msgget failed ,errno:" << errno << endl;
			return -1;
		}
		cout << "child process try to recv msg. " << endl;
		for (int i = 0; i < 10; i++)
		{
			memset(message.mtext, 0, sizeof(message.mtext));
			if (msgrcv(msqid, &message, sizeof(message), 0, 0) < 0)
			{
				cerr << "msgrcv failed, errno:"<<errno << endl;
				break;
			}
			cout << "child process received a message. message ID:" << message.mtype << " content:" << message.mtext << endl;
		}
		exit(0);
	}
	else if (pid > 0)
	{
		int stat;
		msqid = msgget(IPC_SHARED, 0600);
		if (msqid == -1)
		{
			cerr << "msgget failed ,errno:" << errno << endl;
			return -1;
		}
		
		for (int i = 0; i < 10; i++)
		{
			message.mtype = MY_MSG_TYPE;
			sprintf(message.mtext, "my message %d", i);
			if (msgsnd(msqid, &message, strlen(message.mtext), 0) < 0)
			{
				cerr << "msgsnd failed, errno:"<<errno << endl;
				break;
			}
		}
		wait(&stat);
	}
	return 0;
}

int testsem_v()
{
/*	key_t IPC_SHARED = ftok("/tmp/VisualGDB/c/workspace/signal/signal", 1);
	cout << "ipc key is:" << IPC_SHARED << endl;
	int semid;
	union semnu ds[8];
	int pid; 
	struct sembuf[1];
	for (int i = 0; i < 8; i++)
	{
		semnu[i] = 8;
	}
	
	if ((semid = semget(IPC_SHARED, 8, 0600 | IPC_CREAT | IPC_EXCL)) < 0 && errno == EEXIST)
	{
		cerr << "sem already exist..." << endl;
	}
	else if (semid > 0)
	{
		if (semctl(semid, 8, SETALL, &ds) == -1)
		{
			cerr << "set sem value failed, errno:" << errno << endl;
			return -1;
		}
	}
	else
	{
		cerr << "create sem failed, errno:" << errno << endl;
		return -1;
	}
	
	pid = fork();
	if (pid == 0)
	{
		if ((semid = semget(IPC_SHARED, 0, 0600)) < 0 )
		{
			cerr << "semget failed, errno:"<<errno << endl;
			return -1; 
		}
		
		sembuf[0].sem_num = 0;
		sembuf[0].sem_op = 4;
		if (semop(semid, sembuf, 1) < 0)
		{
			cerr << "semop failed, errno:" << errno << endl;
		}
		cout << "child process exit..." << endl;
		exit(0);
	}
	else if (pid > 0)
	{
		if ((semid = semget(IPC_SHARED, 0, 0600)) < 0)
		{
			cerr << "semget failed, errno:" << errno << endl;
			return -1; 
		}
		sleepp(1);
		sembuf[0].sem_num = 0;
		sembuf[0].sem_op = 4;
		if (semop(semid, sembuf, 1) < 0)
		{
			cerr << "semop failed, errno:" << errno << endl;
		}
		cout << "parent process exit..." << endl;
	}*/
	
	return 0;
}