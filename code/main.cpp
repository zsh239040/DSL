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

vector<Error>errors;			//�ʷ������б�
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
		cout << "����������" << index << ":" << endl;
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
			cout << "���û������ڣ�" << endl;
		}
	}
	else
	{
		fstream fout;
		fout.open("./data/errors.txt", ios::out);
		fout << "�����б�" << endl;
		for (auto& error : errors)
		{
			fout << error.pos << "�У�" + error.str + " " + error.info << endl;
		}
	}

	writeUserInfo("./data/user.txt", users);
	system("pause");
	return 0;
}