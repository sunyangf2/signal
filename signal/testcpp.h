#pragma once

#define offsetof(type, member) ((size_t)&((type*)0)->member)
#define container_of(ptr, type, member) ({\
    const typeof( (((type*)0)->member )) *__mptr = (ptr); \
	(type*)((char*)__mptr - offsetof(type, member)); })

class testcpp
{
public:
	testcpp();
	~testcpp();
};

struct typeofexample
{
	int a;
	int b;
};

int testtypeof();