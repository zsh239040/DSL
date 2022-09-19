#pragma once
#include <string>
#include <vector>
#include <map>
using namespace std;

class User
{
private:
	map<string, string> info;	//�û���Ϣ��<���ԣ����Զ�Ӧ��ֵ>
public:
	User(map<string, string> userInfo)
	{
		this->info = userInfo;
	}

	//��ȡ�û�������Ϣ
	map<string, string> getAllInfo();

	//���ݲ���attribute�����û���Ϣ
	string getInfo(string attribute);

	//�����û�����
	void setInfo(map<string, string> info);
};

void readUserInfo(string filePath, vector<User>& users);
void writeUserInfo(string filePath, vector<User>& users); 