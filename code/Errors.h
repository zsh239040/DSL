#pragma once
#include <string>
#include <iostream>

using namespace std;


//错误信息 <错误语句，错误信息，错误所在行数>
class Error
{
public:
	string str;
	string info;
	int pos;
	Error(string str, string info, int pos)
	{
		this->str = str;
		this->info = info;
		this->pos = pos;
	}
	~Error(){}
	//显示错误信息
	void showInfo()
	{
		cout << pos << "行: "<< str << info << endl;
	}
};


