#include "User.h"
#include "ParserTree.h"
#include <fstream>
#include <iostream>

extern string index;
extern vector<string> attributes;


//获取用户所有信息
map<string, string> User::getAllInfo()
{
	return this->info;
}

//根据参数attribute返回用户信息
string User::getInfo(string attribute)
{
	return this->info[attribute];
}

//更新用户数据
void User::setInfo(map<string, string> info)
{
	this->info = info;
}

//读取用户数据
void readUserInfo(string filePath, vector<User>& users)
{
	fstream file;
	file.open(filePath, ios::in);
	while (!file.is_open())
	{
		cout << "用户数据文件打开失败,请输入文件路径：" << endl;
		cin >> filePath;
		file.open(filePath, ios::in);
	}
	string temp;
	file >> index;
	getline(file, temp);
	getline(file, temp);
	attributes = Parser::split(temp, " ");

	while (getline(file, temp))
	{
		vector<string> values = Parser::split(temp, " ");
		map<string, string> info;
		if (values.size() == attributes.size())
		{
			for (int i = 0; i < attributes.size(); i++)
				info[attributes[i]] = values[i];
			User user(info);
			users.push_back(user);
		}
		else
			cout << "用户数据缺失, 应有" << attributes.size() << "项数据" << endl;
	}
	file.close();
}

//写入用户数据
void writeUserInfo(string filePath, vector<User>& users)
{
	fstream file;
	file.open(filePath, ios::out);
	while (!file.is_open())
	{
		cout << "用户数据文件打开失败,请输入文件路径：" << endl;
		cin >> filePath;
		file.open(filePath, ios::in);
	}
	int row = 0;
	string temp;
	file << index << endl;

	file << attributes[0];
	for (int i = 1; i < attributes.size(); i++)
		file << " " << attributes[i];
	file << endl;

	for (int i = 0; i < users.size(); i++)
	{
		file << users[i].getInfo(attributes[0]);
		for (int j = 1; j < attributes.size(); j++)
			file << " " << users[i].getInfo(attributes[j]);
		file << endl;
	}
	file.close();
}