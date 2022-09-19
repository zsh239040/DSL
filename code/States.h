#pragma once
#include <string>
#include <iostream>
#include <vector>
#include <map>
#include <Windows.h>
#include <conio.h>
#include <set>

using namespace std;

typedef int stepId;

typedef string actionName;
typedef vector<string> actionParmVector;
extern map<string, stepId> NameToID;


class Step
{
private:
	stepId id;
	string name;
	set<string> commands;
	set<string> brunchs;
	map<string, string> transtions;
	map<actionName, actionParmVector> actionList;

public:
	Step(string name, stepId id)
	{
		this->name = name;
		this->id = id;
	}
	~Step(){}

	void addCommand(string cmd);
	void addTransition(string cmd, string StepName);
	void addBrunch(string brunch);
	void addAction(actionName name, actionParmVector parm);
	string getName();
	bool isCommandExist(string cmd);
	stepId getTarget(string cmd);
	set<string> getBrunchs();
	map<string, string> getTranstions();
	actionParmVector getActionParms(actionName name);
	string Listen();
};




class Script
{
private:
	vector<Step> steps;
	stepId entry;
	stepId exit;
	vector<string> vars;

public:
	Script()
	{
		this->entry = -1;
		this->exit = -1;
	}
	void addStep(Step step)
	{
		this->steps.push_back(step);
	}
	void setEntry(stepId id)
	{
		this->entry = id;
	}
	void setExit(stepId f)
	{
		this->exit = f;
	}
	bool isExit()
	{
		return this->exit;
	}
	Step getStep(stepId id)
	{
		return this->steps[id];
	}
	stepId getEntry()
	{
		return this->entry;
	}
	vector<string> getVars()
	{
		return this->vars;
	}
	vector <Step>& getSteps()
	{
		return this->steps;
	}
};


