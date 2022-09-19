#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <set>
#include <vector>
#include <algorithm>
#include "ParserTree.h"
#include "Interpreter.h"


using namespace std;

vector<Error>errors;			//词法错误列表
Script script; 
vector<User> users;
vector<string> attributes;
string index;

int main()
{
	readUserInfo("./data/user.txt", users);
	ParserTree parser(script, "./data/proc.txt");
	
	if (errors.empty())
	{
		cout << "请输入您的" << index << ":" << endl;
		string indexValue;
		cin >> indexValue;
		int flag = 0;
		for (auto& user : users)
		{
			if (user.getInfo(index) == indexValue)
			{
				Interpreter interpreter(script, user);
				interpreter.intepret();
				interpreter.writeBack(user);
				flag = 1;
			}
		}
		if (!flag)
		{
			cout << "该用户不存在！" << endl;
		}
	}
	else
	{
		fstream fout;
		fout.open("./data/errors.txt", ios::out);
		fout << "错误列表：" << endl;
		for (auto& error : errors)
		{
			fout << error.pos << "行：" + error.str + " " + error.info << endl;
		}
	}

	writeUserInfo("./data/user.txt", users);
	system("pause");
	return 0;
}