#pragma once
#include <string>
#include <iostream>

using namespace std;


//������Ϣ <������䣬������Ϣ��������������>
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
	//��ʾ������Ϣ
	void showInfo()
	{
		cout << pos << "��: "<< str << info << endl;
	}
};


