#pragma once
#include <string>
#include <iostream>
#include <vector>
#include <map>
#include <set>

using namespace std;

typedef int stepId;

typedef string actionName;
typedef vector<string> actionParmVector;	//存放动作对应的参数
extern map<string, stepId> NameToID;

//状态
class Step
{
private:
	stepId id;
	string name;	//Step名
	set<string> commands;	//该状态包含的指令集
	set<string> brunchs;	//匹配用户回答的关键词集合
	map<string, string> transtions;	//根据指令（commands或brunchs）跳转到下一个状态的状态转移表
	map<actionName, actionParmVector> actionList;	//根据动作名获取所要执行动作的参数

public:
	Step(string name, stepId id)
	{
		this->name = name;
		this->id = id;
	}
	~Step(){}

	//将指令cmd加入指令集commands。
	void addCommand(string cmd);

	//添加转移函数
	void addTransition(string cmd, string StepName);

	//添加用户回答分支
	void addBrunch(string brunch);

	//存储动作和动作参数
	void addAction(actionName name, actionParmVector parm);

	//返回状态名
	string getName();

	//从commands中查找cmd，确认编程人员是否在该Step中添加了cmd指令
	bool isCommandExist(string cmd);

	//根据指令，查询状态转移表transtions，返回转移目标的Id
	stepId getTarget(string cmd);

	//返回用户回答分支，主要用于提示用户，使用户做出有效回答
	set<string> getBrunchs();

	//返回状态转移表
	map<string, string> getTranstions();

	//获取动作名：name对应的动作参数
	actionParmVector getActionParms(actionName name);
};




class Script
{
private:
	vector<Step> steps;	//状态集
	stepId entry;	//脚本执行入口
	stepId exit;	//脚本执行出口，可以不唯一，仅用于判断是否设置了出口
	vector<string> vars;	//变量名

public:
	Script();
	void addStep(Step step);
	void setEntry(stepId id);

	//设置出口，实际上程序可以有多个出口，所以exit仅用于判断是否设置出口。
	void setExit(stepId id);

	//返回id对应的Step
	Step getStep(stepId id);
	stepId getEntry();
	stepId getExit();
	vector<string> getVars();
	vector <Step>& getSteps();
};


