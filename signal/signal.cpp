#include "common.h"
#include "signal.h"
#include "process.h"
#include "testcpp.h"

Sigfunc *oldhandler = NULL;
void handleruser1(int signo)
{
	cout << "receive signal " << signo << endl;
	//sleep(10);
	cout << "quit handleruser1" << endl;
}

void hinfousr1(int signo, siginfo_t * siginfo, void * context)
{
	cout << "siginfo as following:........................" << endl;
	cout << "si_signo:" << siginfo->si_signo << endl;
	cout << "si_errno:" << siginfo->si_errno << endl;
	cout << "si_code:" << siginfo->si_code << endl;
	//仅对SIGCHLD设置
	cout << "si_pid:" << siginfo->si_pid << endl;
	cout << "si_uid:" << siginfo->si_uid << endl;
	cout << "si_status:" << siginfo->si_status << endl;
	//仅对SIGBUS, SIGKILL, SIGFPE, SIGSEGV设置, 引发错误的地址
	cout << "si_addr:" << siginfo->si_addr << endl;
	//sigqueue函数可以设置si_value
	cout << "si_value:" << siginfo->si_value.sival_int << endl;
	
	cout << "context as following......................" << endl;
}

int testsignalfunc()
{
	cout << "signal " << SIGUSR1 << endl;
	oldhandler = signal(SIGUSR1, handleruser1);
	if (oldhandler == SIG_ERR)
	{
		cerr << "signal failed, errno:%v" << errno << endl;
	}
	return 0; 
}
//flags: 
//SA_RESTART, SA_INTERRUPT, SA_SIGINFO, 
//SA_NOCLDSTOP, SA_NOCLDWAIT, SA_NODEFER, 
//SA_RESETHAND, SA_ONSTACK
int sigactionusr1()
{
	struct sigaction act, oact;
	act.sa_handler = handleruser1;
	//SA_NODEFER 调用信号处理函数时不会自动阻塞本信号
	//act.sa_flags |= SA_RESTART | SA_RESETHAND;
	act.sa_flags |= SA_RESTART;
	sigemptyset(&act.sa_mask);
	if (sigaction(SIGUSR1, &act, &oact) < 0)
	{
		cerr << "sigaction failed, errno:%v" << errno << endl;
		return -1;
	}
	return 0; 
}

int signosigchld()
{
	struct sigaction act, oact;
	act.sa_handler = handleruser1;
	//SA_NODEFER 调用信号处理函数时不会自动阻塞本信号
	//act.sa_flags |= SA_RESTART | SA_RESETHAND;
	act.sa_flags |= SA_RESTART | SA_NOCLDSTOP;
	sigemptyset(&act.sa_mask);
	if (sigaction(SIGCHLD, &act, &oact) < 0)
	{
		cerr << "sigaction failed, errno:%v" << errno << endl;
		return -1;
	}
	return 0;
}

int sigsetsigchldignore()
{
	signal(SIGCHLD, SIG_IGN);
	return 0; 
}

int signowaitchild()
{
	struct sigaction act, oact;
	act.sa_handler = handleruser1;
	//SA_NODEFER 调用信号处理函数时不会自动阻塞本信号
	//act.sa_flags |= SA_RESTART | SA_RESETHAND;
	act.sa_flags |= SA_RESTART |SA_NOCLDWAIT;
	sigemptyset(&act.sa_mask);
	if (sigaction(SIGCHLD, &act, &oact) < 0)
	{
		cerr << "sigaction failed, errno:%v" << errno << endl;
		return -1;
	}
	return 0; 
}

int printOldSigmask()
{
	sigset_t newset, oldset;
	if (sigprocmask(SIG_BLOCK, NULL, &oldset) == -1)
	{
		cerr << "get oldset failed, errno:" << errno << endl;
		return -1;
	}
	
	cout << "before block, alarm block status: " << sigismember(&oldset, SIGALRM) << endl;
	
	sigaddset(&newset, SIGALRM);
	if (sigprocmask(SIG_BLOCK, &newset, &oldset) == -1)
	{
		cerr << "block signal failed, errno:" << errno << endl;
		return -1;
	}
	
	if (sigprocmask(SIG_BLOCK, NULL, &oldset) == -1)
	{
		cerr << "block signal failed, errno:" << errno << endl;
		return -1;
	}
	
	cout << "after block, alarm block status: " << sigismember(&oldset, SIGALRM) << endl;
	return 0;
}

int blockAlarm()
{
	sigset_t newset, oldset;
	if (sigprocmask(SIG_BLOCK, NULL, &oldset) == -1)
	{
		cerr << "get oldset failed, errno:" << errno << endl;
		return -1;
	}
	
	cout << "before block, alarm block status: " << sigismember(&oldset, SIGALRM) << endl;
	
	sigaddset(&newset, SIGALRM);
	if (sigprocmask(SIG_BLOCK, &newset, &oldset) == -1)
	{
		cerr << "block signal failed, errno:" << errno << endl;
		return -1;
	}
	
	if (sigprocmask(SIG_BLOCK, NULL, &oldset) == -1)
	{
		cerr << "block signal failed, errno:" << errno << endl;
		return -1;
	}
	
	cout << "after block, alarm block status: " << sigismember(&oldset, SIGALRM) << endl;
	testAlarm();
	
	return 0;
}

int testAlarm()
{
	struct sigaction act, oact;
	act.sa_handler = handleruser1;
	//SA_NODEFER 调用信号处理函数时不会自动阻塞本信号
	//act.sa_flags |= SA_RESTART | SA_RESETHAND;
	act.sa_flags |= SA_RESTART ;
	sigemptyset(&act.sa_mask);
	if (sigaction(SIGALRM, &act, &oact) < 0)
	{
		cerr << "sigaction failed, errno:%v" << errno << endl;
		return -1;
	}
	
	cout << "set alarm 2 secs..." << endl;
	alarm(2);
	return 0; 
}

int sigactioninfo()
{
	struct sigaction act, oact;
	//act.sa_handler = handleruser1;
	//SA_NODEFER 调用信号处理函数时不会自动阻塞本信号
	act.sa_flags |= SA_RESTART | SA_SIGINFO;
	sigemptyset(&act.sa_mask);
	act.sa_sigaction = hinfousr1;
	if (sigaction(SIGUSR1, &act, &oact) < 0)
	{
		cerr << "sigaction failed, errno:%v" << errno << endl;
		return -1;
	}
	return 0; 
}

int waitpid(pid_t id)
{		
	int status;
	int options = 0; //WCONTINUED | WUNTRACED;
	if (waitpid(id, &status, options) < 0)
	{
		cerr << "wait failed, errno:" << errno << endl;
		return -1;	
	}
	cout << "normal quit:" << WIFEXITED(status) << endl;
	cout << "signal stop:" << WIFSIGNALED(status) << endl;
	cout << "stopped: " << WIFSTOPPED(status) << endl;
	cout << "if continued:" << WIFCONTINUED(status) << endl;
	cout << "parent quit" << endl;
		
	//		if (waitpid(id, &status, options) < 0)
	//		{
	//			cerr << "wait failed, errno:" << errno << endl;
	//			return -1;	
	//		}
	//		cout << "normal quit:" << WIFEXITED(status) << endl;
	//		cout << "signal stop:" << WIFSIGNALED(status) << endl;
	//		cout << "stopped: " << WIFSTOPPED(status) << endl;
	//		cout << "if continued:" << WIFCONTINUED(status) << endl;
	//		cout << "parent quit" << endl;
	return 0;
}
int waittid(pid_t id)
{
	int options = WSTOPPED;        //WCONTINUED | WEXITED|WNOHANG|WNOWAIT;
	siginfo_t siginfo;
	if (waitid(P_PID, id, &siginfo, options) < 0)
	{
		cerr << "wait failed, errno:" << errno << endl;
		return -1;	
	}
	cout << "signal no:" << siginfo.si_signo << endl;
	cout <<  "signal errno:" << siginfo.si_errno << endl;
	cout << "signal code:" << siginfo.si_code << endl;
	cout << "signal pid:" << siginfo.si_pid << endl;
	cout << "signal uid:" << siginfo.si_uid << endl;
	cout << "signal address:" << siginfo.si_addr << endl;
	cout << "normal quit:" << WIFEXITED(siginfo.si_status) << endl;
	cout << "signal stop:" << WIFSIGNALED(siginfo.si_status) << endl;
	cout << "stopped: " << WIFSTOPPED(siginfo.si_status) << endl;
	cout << "if continued:" << WIFCONTINUED(siginfo.si_status) << endl;
	cout << "parent quit" << endl;
}

int testsigchld()
{
	pid_t id;
	id = fork();
	//sigsetsigchldignore();
	//signowaitchild();
	if (id == 0)
	{
		//pause();
		cout << "child process quit..." << endl;
	}
	else
	{
		cout << "in parent.." << endl;
		//pause();
		//int status;
		//				if (wait(&status) < 0)
		//				{
		//					cerr << "wait failed, errno:" << errno << endl;
		//					return -1;
		//				}
		//waitpid(id);
		//waittid(id);
		pause();
        
	}
	return 0;
}

int testsuspend()
{
	//设置SIGUSR1响应函数
	oldhandler = signal(SIGUSR1, handleruser1);
	if (oldhandler == SIG_ERR)
	{
		cerr << "signal failed, errno:%v" << errno << endl;
		return -1;
	}
	
	oldhandler = signal(SIGUSR2, handleruser1);
	if (oldhandler == SIG_ERR)
	{
		cerr << "signal failed, errno:%v" << errno << endl;
		return -1;
	}
	
	sigset_t newset, afterset;
	sigemptyset(&newset);
	sigaddset(&newset, SIGUSR1);
	sigaddset(&newset, SIGUSR2);
	
	if (sigprocmask(SIG_BLOCK, &newset, NULL) == -1)
	{
		cerr << "sigprocmask failed, errno:" << errno << endl;
		return -1;
	}
	
	sigfillset(&afterset);
	sigdelset(&afterset, SIGUSR1);
	sigdelset(&afterset, SIGUSR2);
	cout << "sigsuspend..." << endl;
	if (sigsuspend(&afterset) == -1 && errno == EINTR)
	{
		cout << "sigsuspend was interupted..." << endl;
	}
	cout << "quit testsuspend" << endl;
}

int testsigwait()
{
	//设置SIGUSR1响应函数
	struct sigaction act, oact;
	act.sa_handler = handleruser1;
	//SA_NODEFER 调用信号处理函数时不会自动阻塞本信号
	act.sa_flags = SA_RESTART ;
	sigemptyset(&act.sa_mask);
	//act.sa_sigaction = hinfousr1;
	if (sigaction(SIGUSR1, &act, &oact) < 0)
	{
		cerr << "sigaction failed, errno:%v" << errno << endl;
		return -1;
	}
	
	if (sigaction(SIGUSR2, &act, &oact) < 0)
	{
		cerr << "sigaction failed, errno:%v" << errno << endl;
		return -1;
	}
	
	sigset_t newset, afterset;
	sigemptyset(&newset);
	sigaddset(&newset, SIGUSR1);
	sigaddset(&newset, SIGUSR2);
	//一般使用sigwait都要阻塞信号
//	if (sigprocmask(SIG_BLOCK, &newset, NULL) == -1)
//	{
//		cerr << "sigprocmask failed, errno:" << errno << endl;
//		return -1;
//	}
	
	int signo = 0;
	if (sigwait(&newset, &signo) == -1)
	{
		cerr << "sigwait failed, errno:" << errno << endl;
		return -1;
	}
	if (signo == SIGUSR1)
	{
		cout <<"wait SIGUSR1"<< endl;
	}
	else if (signo == SIGUSR2)
	{
		cout << "wait SIGUSR2" << endl;
	}
	
	return 0;
}

map<int, string> threadmap;

void multithreadhandleruser1(int signo)
{
	if (signo == SIGUSR1)
	{
		cout << "thread[" << threadmap[pthread_self()] << "] receive signal SIGUSR1..." << endl;
	}
}

void* signalthread(void *para)
{
	pthread_t myid = pthread_self();
	if (threadmap[myid] == "p1")
	{
		//设置SIGUSR1响应函数
        struct sigaction act, oact;
		act.sa_handler = multithreadhandleruser1;
		//SA_NODEFER 调用信号处理函数时不会自动阻塞本信号
		act.sa_flags = 0; //SA_RESTART;
		sigemptyset(&act.sa_mask);
		//act.sa_sigaction = hinfousr1;
		if(sigaction(SIGUSR1, &act, &oact) < 0)
		{
			cerr << "sigaction failed, errno:%v" << errno << endl;
			return (void*) -1;
		}
	}
	
	/*/if (threadmap[myid]!="p4")
	{
		sigset_t newset;
		sigaddset(&newset, SIGUSR1);
		if (pthread_sigmask(SIG_BLOCK, &newset, NULL) == -1)
			//if(sigprocmask(SIG_BLOCK, &newset, NULL) == -1)
		{
			cerr << "pthread_sigmask failed, errno:" << errno << endl;
			return (void*)-1;
		}
	}*/
	cout << "thread[" << threadmap[myid] << "] is running ...." << endl ;
	char buff[MAXLINE] = { 0 };
	//sleep(2);
	int n = read(STDIN_FILENO, buff, MAXLINE);
	if (n == -1 && errno == EINTR)
	{
		cout << "thread[" << threadmap[myid] << "] is interupted ...." << endl;
	}
	cout << "thread[" << threadmap[myid] << "] quit ...." << endl;
	return (void *)-1;
}

int testmultthreadssig()
{
	pthread_t p1, p2, p3, p4;
	cout << "main thread is:" << pthread_self() << endl;
	threadmap.insert(pair<int, string>(pthread_self(), "main thread"));
	
	//设置SIGUSR1响应函数
    /*struct sigaction act, oact;
	act.sa_handler = multithreadhandleruser1;
	//SA_NODEFER 调用信号处理函数时不会自动阻塞本信号
	act.sa_flags = 0;  //SA_RESTART;
	sigemptyset(&act.sa_mask);
	//act.sa_sigaction = hinfousr1;
	if(sigaction(SIGUSR1, &act, &oact) < 0)
	{
		cerr << "sigaction failed, errno:%v" << errno << endl;
		return -1;
	}*/
	
	int errcode = 0;
	errcode = pthread_create(&p1, NULL, signalthread, NULL);
	if (errcode != 0)
	{
		cerr << "create thread failed, errcode:" << errcode << endl;
		return -1;
	}
	threadmap.insert(pair<int, string>(p1, "p1"));
	
	errcode = pthread_create(&p2, NULL, signalthread, NULL);
	if (errcode != 0)
	{
		cerr << "create thread failed, errcode:" << errcode << endl;
		return -1;
	}
	threadmap.insert(pair<int, string>(p2, "p2"));
	errcode = pthread_create(&p3, NULL, signalthread, NULL);
	if (errcode != 0)
	{
		cerr << "create thread failed, errcode:" << errcode << endl;
		return -1;
	}
	threadmap.insert(pair<int, string>(p3, "p3"));
	errcode = pthread_create(&p4, NULL, signalthread, NULL);
	if (errcode != 0)
	{
		cerr << "create thread failed, errcode:" << errcode << endl;
		return -1;
	}
	threadmap.insert(pair<int, string>(p4, "p4"));
	
	sigset_t newset;
	/*sigaddset(&newset, SIGUSR1);
	if (pthread_sigmask(SIG_BLOCK, &newset, NULL) == -1)
	//if(sigprocmask(SIG_BLOCK, &newset, NULL) == -1)
	{
		cerr << "pthread_sigmask failed, errno:"<<errno << endl;
		return -1;
	}*/
	
	cout << "created 4 threads" << endl;
	
	sleep(2);
	pthread_kill(p2, SIGUSR1);
	sleep(2);
	pthread_kill(p4, SIGUSR1);
	sleep(2);
	pthread_kill(p3, SIGUSR1);
	sleep(2);
	pthread_kill(p1, SIGUSR1);
	sleep(2);
	
	errcode = pthread_join(p1, NULL);
    if(errcode != 0)
	{
		cerr << "join thread 1 failed, errcode:" << errcode << endl ;
		return - 1 ;
	}
	cout << "done with join thread 1 " << endl;
	
	errcode = pthread_join(p2, NULL);
    if(errcode != 0)
	{
		cerr << "join thread 2 failed, errcode:" << errcode << endl;
		return - 1 ;
	}
	cout << "done with join thread 2 " << endl;
	
	errcode = pthread_join(p3, NULL);
    if(errcode != 0)
	{
		cerr << "join thread 3 failed, errcode:" << errcode << endl;
		return - 1 ;
	}
	
	cout << "done with join thread 3 " << endl;
	errcode = pthread_join(p4, NULL);
    if(errcode != 0)
	{
		cerr << "join thread 4 failed, errcode:" << errcode << endl;
		return - 1 ;
	}
	cout << "done with join thread 4 " << endl;
	
	return 0;
}

void main_wait()
{
	char buff[1024] = { 0 };
	
	int n = read(STDIN_FILENO, buff, sizeof(buff));
	//int n = pause();
	if(n == -1 && errno == EINTR)
	{ 
		cout << "read was interrupted..." << endl;
	}
	else if(n == -1)
	{
		cerr << "read failed , errno :" << errno << endl;
	}
}

int gjumpv = 10;
jmp_buf env;
int jumpfunc(int *sv, int * hv)
{
	if (rand() % 2 == 0)
	{
		gjumpv += 2;
		*sv += 2;
		*hv += 2;
		longjmp(env, 2);
	}
	else
	{
		gjumpv += 5;
		*sv += 5;
		*hv += 5;
		longjmp(env, 1);
	}
}
int testlongjump()
{
	int *hjumpv = new int(10);
	int sjumpv = 10;
	
	int position = 0;
	position = setjmp(env);
	if (position != 0)
	{
		cout << "return from code:" << position << endl;
		cout << "stack v:" << sjumpv << " heap v:" << *hjumpv << " global v:" << gjumpv << endl;
		if (position == 1)
		{
			return 0;
		}
	}
	jumpfunc(&sjumpv, hjumpv);
	return 0; 
}

int main(int argc, char *argv[])
{
	//int * p= NULL;
	//*p = 123;
	if (argc == 1)
	{
		//sigactioninfo();
		//testAlarm();
		//printOldSigmask();
		//blockAlarm();
		//testsuspend();
		//testsigwait();
		//testmultthreadssig();
		//testlongjump();
		 //testsystem();
		//testsignalfunc();
		//signal(SIGUSR1, handleruser1);
		//testvfork();
		//testthreadjoin();
		//testmultithreadfork();
		//testthreadcancel();
		//testthreadattr();
		//testatfork();
		//testthreadprivatekey();
		//testpthreadonce();
		//testmutext();
		//testshmmtext();
		//testrobustmutex();
		//testshmlifespan0();
		//testconv();
		//testbarrier();
		//testpipe();
		//testpopen();
		//testfifo();
		//testmsgqueue();
//		testsem_v();
		
		//testcpp
		testtypeof();
		
		//是否需要在main函数里wait
		//main_wait();
        cout << "quit main" << endl;
	}
	else if (argc == 2)
	{
		int signo;
		if (sscanf(argv[1], "%d", &signo) == 1)
		{
			sigactioninfo();
			sigval val;
			val.sival_int = 17;
			sigqueue(getpid(), signo, val);
			string in;
			cin >> in;
		}
		else
		{
			cout << "testsigchld" << endl;
			testsigchld();
		}
	}else if(argc == 3)
	{
		testshmlifespan1();
	}

	return 0;
}