#include "Script.h"

//��ָ��cmd����ָ�commands��
void Step::addCommand(string cmd)
{
	this->commands.insert(cmd);
}

//���ת�ƺ���
void Step::addTransition(string cmd, string StepName)
{
	this->addCommand(cmd);
	this->transtions[cmd] = StepName;
}

//����û��ش��֧
void Step::addBrunch(string brunch)
{
	this->brunchs.insert(brunch);
}

//�洢��������������
void Step::addAction(actionName name, actionParmVector parms)
{
	this->addCommand(name);
	this->actionList[name] = parms;
}

//����״̬��
string Step::getName()
{
	return this->name;
}

//��commands�в���cmd��ȷ�ϱ����Ա�Ƿ��ڸ�Step�������cmdָ��
bool Step::isCommandExist(string cmd)
{
	if (commands.find(cmd) != commands.end())
		return true;
	else
		return false;
}

//����ָ���ѯ״̬ת�Ʊ�transtions������ת��Ŀ���Id
stepId Step::getTarget(string cmd)
{
	name = this->transtions[cmd];
	auto it = NameToID.find(name);
	if (it == NameToID.end())
		return -1;
	else
		return it->second;
}

//�����û��ش��֧����Ҫ������ʾ�û���ʹ�û�������Ч�ش�
set<string> Step::getBrunchs()
{
	return this->brunchs;
}

//����״̬ת�Ʊ�
map<string, string> Step::getTranstions()
{
	return this->transtions;
}

//��ȡ��������name��Ӧ�Ķ�������
actionParmVector Step::getActionParms(actionName name)
{
	return this->actionList[name];
}

Script::Script()
{
	this->entry = -1;
	this->exit = -1;
}

void Script::addStep(Step step)
{
	this->steps.push_back(step);
}

void Script::setEntry(stepId id)
{
	this->entry = id;
}

//���ó��ڣ�ʵ���ϳ�������ж�����ڣ�����exit�������ж��Ƿ����ó��ڡ�
void Script::setExit(stepId id)
{
	this->exit = id;
}

//����id��Ӧ��Step
Step Script::getStep(stepId id)
{
	return this->steps[id];
}

stepId Script::getEntry()
{
	return this->entry;
}

stepId Script::getExit()
{
	return this->exit;
}

vector<string> Script::getVars()
{
	return this->vars;
}

vector <Step>& Script::getSteps()
{
	return this->steps;
}