#pragma once

//使用signal函数响应SIGUSR1信号
int testsignalfunc();
//使用sigaction函数响应SIGUSR1信号
int sigactionusr1();
//使用设置SIGINFO
int sigactioninfo();
//测试SIGCHLD相关的信号
int testsigchld();
int waitpid(pid_t id);
int waittid(pid_t id);
//子进程退出父进程不需要wait,子进程也不变僵尸进程
int signowaitchild();
int signosigchld();
//测试alarm
int testAlarm();
//测试sigpromask
int printOldSigmask();
int blockAlarm();
//测试sigsuspend
int testsuspend();
//测试sigwait
int testsigwait();
//测试多线程的信号
int testmultthreadssig();
//测试longjump
int testlongjump();

