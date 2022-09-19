#pragma once
#include "Script.h"
#include "Errors.h"
#include "User.h"
#include <fstream>
#include <Windows.h>

extern string index;

class Interpreter
{
private:
	fstream logFile;		//��־�ļ�
	Script& script;			//�����õ��Ľű��﷨��
	stepId currentStepId;	//��ǰStep��״̬�����裩��Id
	stepId preStepId;		//��һ��Step��Id
	bool interrupt;			//����ʹ��Backָ��ʱ�����Speak��ֱ�ӽ���Listen
	string answer;			//�洢�û��ش�
	map<string, string> userInfo;	//��ǰ�û���Ϣ
	
public:
	//���ýű��ļ��������û�����
	Interpreter(Script& script, User user): script(script)
	{
		logFile.open("./log/interpretLog.txt", ios::out|ios::app);
		preStepId = -1;
		interrupt = false;
		currentStepId = this->script.getEntry();
		this->userInfo = user.getAllInfo();
		debug("User: "+ user.getInfo(index));
	}
	~Interpreter()
	{
		debug("�û�: "+ userInfo[index] +"�˳�\n");
		logFile << endl << endl;
		logFile.close();
	}
	//�����ű�
	void intepret();

	//��� expression ���
	void expressProc(vector<string> expression);

	string listenProc(vector<string> parms);

	//�Ƚ��û��Ļش�answer���������parms����Ӧ���û����ݣ�ƥ��ɹ�������
	bool compareProc(string answer,  vector<string> parms);

	//parms��¼�������ʽ�����޼Ӽ�
	void computeProc(vector<string> parms);

	//���û��ش�answer�����뵽parms��Ӧ���û�������
	void addProc(vector<string> parms);

	//���û��ش��parms��Ӧ���û�������ȥ��
	void delProc(vector<string> parms);

	//��ȡ�û��ش��еĹؼ���, brunchsΪ��ǰ״̬��ʶ��Ĺؼ����б�
	string getKeywords(string answer, set<string> brunchs);

	//����ʱ����д���û�����
	void writeBack(User& user);

	//��־��Ϣ
	void debug(string info);
};

