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
	fstream logFile;		//日志文件
	Script& script;			//分析得到的脚本语法树
	stepId currentStepId;	//当前Step（状态、步骤）的Id
	stepId preStepId;		//上一个Step的Id
	bool interrupt;			//用于使用Back指令时，打断Speak，直接进入Listen
	string answer;			//存储用户回答
	map<string, string> userInfo;	//当前用户信息
	
public:
	//引用脚本文件，读入用户数据
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
		debug("用户: "+ userInfo[index] +"退出\n");
		logFile << endl << endl;
		logFile.close();
	}
	//解析脚本
	void intepret();

	//表达 expression 语句
	void expressProc(vector<string> expression);

	string listenProc(vector<string> parms);

	//比较用户的回答（answer）与参数（parms）对应的用户数据，匹配成功返回真
	bool compareProc(string answer,  vector<string> parms);

	//parms记录算数表达式，仅限加减
	void computeProc(vector<string> parms);

	//将用户回答（answer）加入到parms对应的用户数据中
	void addProc(vector<string> parms);

	//将用户回答从parms对应的用户数据中去除
	void delProc(vector<string> parms);

	//获取用户回答中的关键词, brunchs为当前状态可识别的关键词列表
	string getKeywords(string answer, set<string> brunchs);

	//将临时数据写回用户数据
	void writeBack(User& user);

	//日志信息
	void debug(string info);
};

