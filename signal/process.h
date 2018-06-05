#pragma once
class process
{
public:
	process();
	virtual ~process();
};

//实现system函数
int mysystem(const char *cmdstring);
int testsystem();
//测试vfork函数
int testvfork();
//测试thread_join函数
int testthreadjoin();
//测试多线程的fork
int testmultithreadfork();
//测试pthreadcancel
int testthreadcancel();
//测试线程属性
int testthreadattr();
//测试atfork
int testatfork();
//测试线程私有变量
int testthreadprivatekey();
//测试pthread_once
int testpthreadonce();

int initthreads(int threadnum, void* (*fname)(void*));
//测试mutext
int testmutext();
//使用共享内存测试进程间共享的mutex
int testshmmtext();
//测试mutex的健壮属性和一致性属性, 当进程没有释放的时候, 另外一个进程要对mutex进行恢复.
int testrobustmutex();
//测试共享内存IPC的生存周期
int testshmlifespan(int &shmid, void *&shmptr);
int testshmlifespan0();//????
int testshmlifespan1();
//测试条件变量
int testconv();
//测试屏障
int testbarrier();
//测试记录锁 ????
//测试管道+popen, pclose
int testpipe();
//管道实现一个协同进程
int testpopen();
//测试FIFO
int testfifo();
//测试消息队列
int testmsgqueue();
//测试system v信号量 undo
int testsem_v();
//测试posix信号量 进程没有释放就退出的情况
//测试posix共享内存