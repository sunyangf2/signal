#pragma once
class process
{
public:
	process();
	virtual ~process();
};

//ʵ��system����
int mysystem(const char *cmdstring);
int testsystem();
//����vfork����
int testvfork();
//����thread_join����
int testthreadjoin();
//���Զ��̵߳�fork
int testmultithreadfork();
//����pthreadcancel
int testthreadcancel();
//�����߳�����
int testthreadattr();
//����atfork
int testatfork();
//�����߳�˽�б���
int testthreadprivatekey();
//����pthread_once
int testpthreadonce();

int initthreads(int threadnum, void* (*fname)(void*));
//����mutext
int testmutext();
//ʹ�ù����ڴ���Խ��̼乲���mutex
int testshmmtext();
//����mutex�Ľ�׳���Ժ�һ��������, ������û���ͷŵ�ʱ��, ����һ������Ҫ��mutex���лָ�.
int testrobustmutex();
//���Թ����ڴ�IPC����������
int testshmlifespan(int &shmid, void *&shmptr);
int testshmlifespan0();//????
int testshmlifespan1();
//������������
int testconv();
//��������
int testbarrier();
//���Լ�¼�� ????
//���Թܵ�+popen, pclose
int testpipe();
//�ܵ�ʵ��һ��Эͬ����
int testpopen();
//����FIFO
int testfifo();
//������Ϣ����
int testmsgqueue();
//����system v�ź��� undo
int testsem_v();
//����posix�ź��� ����û���ͷž��˳������
//����posix�����ڴ�