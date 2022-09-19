#include "Script.h"

//将指令cmd加入指令集commands。
void Step::addCommand(string cmd)
{
	this->commands.insert(cmd);
}

//添加转移函数
void Step::addTransition(string cmd, string StepName)
{
	this->addCommand(cmd);
	this->transtions[cmd] = StepName;
}

//添加用户回答分支
void Step::addBrunch(string brunch)
{
	this->brunchs.insert(brunch);
}

//存储动作即动作参数
void Step::addAction(actionName name, actionParmVector parms)
{
	this->addCommand(name);
	this->actionList[name] = parms;
}

//返回状态名
string Step::getName()
{
	return this->name;
}

//从commands中查找cmd，确认编程人员是否在该Step中添加了cmd指令
bool Step::isCommandExist(string cmd)
{
	if (commands.find(cmd) != commands.end())
		return true;
	else
		return false;
}

//根据指令，查询状态转移表transtions，返回转移目标的Id
stepId Step::getTarget(string cmd)
{
	name = this->transtions[cmd];
	auto it = NameToID.find(name);
	if (it == NameToID.end())
		return -1;
	else
		return it->second;
}

//返回用户回答分支，主要用于提示用户，使用户做出有效回答
set<string> Step::getBrunchs()
{
	return this->brunchs;
}

//返回状态转移表
map<string, string> Step::getTranstions()
{
	return this->transtions;
}

//获取动作名：name对应的动作参数
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

//设置出口，实际上程序可以有多个出口，所以exit仅用于判断是否设置出口。
void Script::setExit(stepId id)
{
	this->exit = id;
}

//返回id对应的Step
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