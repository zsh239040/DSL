#pragma once
#include <string>
#include <vector>
#include <map>
using namespace std;

class User
{
private:
	map<string, string> info;	//用户信息：<属性，属性对应的值>
public:
	User(map<string, string> userInfo)
	{
		this->info = userInfo;
	}

	//获取用户所有信息
	map<string, string> getAllInfo();

	//根据参数attribute返回用户信息
	string getInfo(string attribute);

	//更新用户数据
	void setInfo(map<string, string> info);
};

void readUserInfo(string filePath, vector<User>& users);
void writeUserInfo(string filePath, vector<User>& users); 