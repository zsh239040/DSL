#pragma once
#include <string>
#include <iostream>
#include <vector>
#include <map>
#include <set>

using namespace std;

typedef int stepId;

typedef string actionName;
typedef vector<string> actionParmVector;	//��Ŷ�����Ӧ�Ĳ���
extern map<string, stepId> NameToID;

//״̬
class Step
{
private:
	stepId id;
	string name;	//Step��
	set<string> commands;	//��״̬������ָ�
	set<string> brunchs;	//ƥ���û��ش�Ĺؼ��ʼ���
	map<string, string> transtions;	//����ָ�commands��brunchs����ת����һ��״̬��״̬ת�Ʊ�
	map<actionName, actionParmVector> actionList;	//���ݶ�������ȡ��Ҫִ�ж����Ĳ���

public:
	Step(string name, stepId id)
	{
		this->name = name;
		this->id = id;
	}
	~Step(){}

	//��ָ��cmd����ָ�commands��
	void addCommand(string cmd);

	//���ת�ƺ���
	void addTransition(string cmd, string StepName);

	//����û��ش��֧
	void addBrunch(string brunch);

	//�洢�����Ͷ�������
	void addAction(actionName name, actionParmVector parm);

	//����״̬��
	string getName();

	//��commands�в���cmd��ȷ�ϱ����Ա�Ƿ��ڸ�Step�������cmdָ��
	bool isCommandExist(string cmd);

	//����ָ���ѯ״̬ת�Ʊ�transtions������ת��Ŀ���Id
	stepId getTarget(string cmd);

	//�����û��ش��֧����Ҫ������ʾ�û���ʹ�û�������Ч�ش�
	set<string> getBrunchs();

	//����״̬ת�Ʊ�
	map<string, string> getTranstions();

	//��ȡ��������name��Ӧ�Ķ�������
	actionParmVector getActionParms(actionName name);
};




class Script
{
private:
	vector<Step> steps;	//״̬��
	stepId entry;	//�ű�ִ�����
	stepId exit;	//�ű�ִ�г��ڣ����Բ�Ψһ���������ж��Ƿ������˳���
	vector<string> vars;	//������

public:
	Script();
	void addStep(Step step);
	void setEntry(stepId id);

	//���ó��ڣ�ʵ���ϳ�������ж�����ڣ�����exit�������ж��Ƿ����ó��ڡ�
	void setExit(stepId id);

	//����id��Ӧ��Step
	Step getStep(stepId id);
	stepId getEntry();
	stepId getExit();
	vector<string> getVars();
	vector <Step>& getSteps();
};


