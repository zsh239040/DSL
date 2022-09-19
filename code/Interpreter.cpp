#include "Interpreter.h"
#include <conio.h>
#include <stdio.h>
#include <Windows.h>

//解析脚本
void Interpreter::intepret()
{
	vector<Step>& steps = script.getSteps();
	while (true) //解析Step里存储的命令的参数列表： actionParms
	{
		//正在执行中的Step
		Step &currentStep = steps[currentStepId];
		vector<string> expression = currentStep.getActionParms("Speak");
		debug(currentStep.getName() + ": ");

		//Back指令可以返回到上一个Step，并置interrupt为true，打断Speak，直接进入Listen
		if (!interrupt && !expression.empty())
		{
			expressProc(expression);
		}
		if (interrupt)
		{
			interrupt = false;
		}

		//以下三条为执行指令
		if (currentStep.isCommandExist("Compute"))
			computeProc(currentStep.getActionParms("Compute"));

		if (currentStep.isCommandExist("Add"))
			addProc(currentStep.getActionParms("Add"));

		if (currentStep.isCommandExist("Delete"))
			delProc(currentStep.getActionParms("Delete"));

		
		//通过用户answer确认条件跳转的目标
		if (currentStep.isCommandExist("Listen"))
		{
			string sentence = listenProc(currentStep.getActionParms("Listen"));
			answer = getKeywords(sentence, currentStep.getBrunchs());
			debug("User answer: " + answer);

			if (answer.empty())
			{
				if (currentStep.isCommandExist("Silence"))
				{
					preStepId = currentStepId;
					currentStepId = currentStep.getTarget("Silence");
					continue;
				}
				else
				{
					debug(currentStep.getName()+ ": 函数缺少“Silence”入口");
					cout << currentStep.getName() << ": 函数缺少“Silence”入口" << endl;
				}
			}
			//answer不为空，查看answer是否匹配brunch（commands包含程序员设置的brunch）
			else if (currentStep.isCommandExist(answer))
			{
				preStepId = currentStepId;
				currentStepId = currentStep.getTarget(answer);
				continue;
			}
			//匹配brunch失败，若脚本设置了compare命令，则answer可能为可变数据，需要进行比较
			else if (currentStep.isCommandExist("Compare"))
			{
				bool ret = compareProc(answer, currentStep.getActionParms("Compare"));
				if (ret && currentStep.isCommandExist("Right"))
				{
					preStepId = currentStepId;
					currentStepId = currentStep.getTarget("Right");
					continue;
				}
				else if (!ret && currentStep.isCommandExist("Wrong"))
				{
					preStepId = currentStepId;
					currentStepId = currentStep.getTarget("Wrong");
					continue;
				}
			}
			//以上都匹配失败，则进入default
			else if(currentStep.isCommandExist("Default"))
			{
				preStepId = currentStepId;
				currentStepId = currentStep.getTarget("Default");
				continue;
			}
			else
			{
				debug(currentStep.getName() + "函数缺少“Default”入口");
				cout << currentStep.getName() << ": 函数缺少“Default”入口" << endl;
				return;
			}
		}
		//如果该Step没有设置listen，则进入default，实现默认跳转
		else if (currentStep.isCommandExist("Default"))
		{
			//interrupt = true;
			preStepId = currentStepId;
			currentStepId = currentStep.getTarget("Default");
			continue;
		}

		//无条件跳转，返回上一个Step，并打断Speak
		if (currentStep.isCommandExist("Back"))
		{
			interrupt = true;
			currentStepId = preStepId;
			continue;
		}

		//无条件跳转，退出。
		if (currentStep.isCommandExist("Exit"))
		{
			return;
		}
	}
}

//表达 expression 语句
void Interpreter::expressProc(vector<string> expression)
{

	debug("");
	cout << endl;
	string sentence;
	for (size_t i = 0; i < expression.size(); i++)
	{
		if (expression[i] == "&")
		{
			if (expression[i + 1] == "recommend")
			{
				set<string> brunchs = script.getStep(preStepId).getBrunchs();
				for (auto& select : brunchs)
					sentence += " " + select;
			}
			else
				sentence += userInfo[expression[i + 1]];
			i += 2;
		}
		else if(expression[i] != "+")
		{
			sentence += expression[i];
		}
	}
	debug("Speak " + sentence);
	cout << sentence << endl;
}

string Interpreter::listenProc(vector<string> parms)
{
	float beginTime = atoi(parms[0].c_str()) * 20;
	float endTime = atoi(parms[1].c_str()) * 20;
	string sentence;
	if (beginTime >= 0 && endTime > 0)
	{
		while (endTime >= 0)
		{
			//至少听 beginTime 
			if (_kbhit() && beginTime <= 0)
			{
				char ch = _getch();
				cout << ch;
				if (ch == 13)	//遇到回车
				{
					break;
				}
				sentence.push_back(ch);
			}
			Sleep(50);
			endTime--;
			beginTime--;
		}
		Sleep(1000);
		cout << endl;
	}
	else
	{
		debug("Listen参数有误，请保证第一个参数非负，第二个参数不小于1");
		cout << "Listen参数有误，请保证第一个参数非负，第二个参数不小于1" << endl;
	}
	return sentence;
}

//比较用户的回答（answer）与参数（parms）对应的用户数据，匹配成功返回真
bool Interpreter::compareProc(string answer,  vector<string> parms)
{
	bool isRight = false;
	debug("Compare  answer:" + answer + "  and  " + parms[1] + ": " + userInfo[parms[1]]);
	
	if (userInfo[parms[1]].find("|") != string::npos)
	{
		int pos = userInfo[parms[1]].find(answer);
		if (pos != userInfo[parms[1]].npos)
		{
			//answer应为多值参数中的某一个值，而不能是多值参数中某个值的一部分。
			if (userInfo[parms[1]][pos + answer.size()] == '|')
				isRight = true;
		}
	}
	else if (userInfo[parms[1]] == answer)
		isRight = true;

	return isRight;
}

//parms记录算数表达式，仅限加减
void Interpreter::computeProc(vector<string> parms)
{
	float balance = atoi(userInfo[parms[1]].c_str());
	float deta;
	if (parms[3] == "&") //参数为变量
		deta = atoi(userInfo[parms[4]].c_str());
	else  //参数为立即数
		deta = atoi(parms[3].c_str());
	char str[100];
	sprintf_s(str, 100, "%.2f", balance + deta);
	userInfo[parms[1]] = str;
	debug("Compute " + parms[1] + ": " + userInfo[parms[1]] + " + " + to_string(deta) + " = " + userInfo[parms[1]]);
}

//将用户回答（answer）加入到parms对应的用户数据中
void Interpreter::addProc(vector<string> parms)
{
	debug("Add " + answer + " to " + parms[1] + ": " + userInfo[parms[1]]);
	if(userInfo[parms[1]].find("|") != userInfo[parms[1]].npos)
		userInfo[parms[1]] += answer + "|";
	else
	{
		cout << parms[1] << " 不是多值属性，无法进行添加操作！" << endl;
		debug(parms[1] + " 不是多值属性，无法进行添加操作！");
	}
}

//将用户回答从parms对应的用户数据中去除
void Interpreter::delProc(vector<string> parms)
{
	debug("Del " + answer + " from " + parms[1] + ": " + userInfo[parms[1]]);
	if (userInfo[parms[1]].find("|") != userInfo[parms[1]].npos)
	{
		size_t pos = userInfo[parms[1]].find(answer);
		userInfo[parms[1]].erase(pos, answer.length() + 1);
	}
	else
	{
		cout << parms[1] << " 不是多值属性，无法进行删除操作！" << endl;
		debug(parms[1] + " 不是多值属性，无法进行删除操作！");
	}
}

//获取用户回答中的关键词
string Interpreter::getKeywords(string answer, set<string> brunchs)
{
	for (auto& brunch : brunchs)
	{
		//只要成功匹配到一个关键词则返回关键字
		if (answer.find(brunch) != answer.npos)
			return brunch;
	}
	//没匹配到则返回原字符串
	return answer;
}

//将临时数据写回用户数据
void Interpreter::writeBack(User& user)
{
	user.setInfo(userInfo);
}

//日志信息
void Interpreter::debug(string info)
{
	SYSTEMTIME time;
	GetLocalTime(&time);
	logFile << time.wMonth << "-" << time.wDay << " " << time.wHour << ":" << time.wMinute << ":" << time.wSecond;
	logFile << "  " << info << endl;
}