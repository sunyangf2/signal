#pragma once

//ʹ��signal������ӦSIGUSR1�ź�
int testsignalfunc();
//ʹ��sigaction������ӦSIGUSR1�ź�
int sigactionusr1();
//ʹ������SIGINFO
int sigactioninfo();
//����SIGCHLD��ص��ź�
int testsigchld();
int waitpid(pid_t id);
int waittid(pid_t id);
//�ӽ����˳������̲���Ҫwait,�ӽ���Ҳ���佩ʬ����
int signowaitchild();
int signosigchld();
//����alarm
int testAlarm();
//����sigpromask
int printOldSigmask();
int blockAlarm();
//����sigsuspend
int testsuspend();
//����sigwait
int testsigwait();
//���Զ��̵߳��ź�
int testmultthreadssig();
//����longjump
int testlongjump();

