#include "testcpp.h"
#include <iostream>
using namespace std;


testcpp::testcpp()
{
}


testcpp::~testcpp()
{
}


int testtypeof()
{
	cout << "offset of B in typeofexample:" << offsetof(typeofexample, b) << endl;
	typeofexample temp;
	cout << "address of temp:" << &temp << endl;
	cout << "address of temp.b:" << &temp.b << endl;
	cout << "expect address of temp:" << container_of(&(temp.b), typeofexample, b) << endl;
	return 0;
}