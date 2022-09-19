#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <set>
#include <map>

using namespace std;

class Parser
{
private:
	fstream file;	//打开设计的文法文件
	fstream debug;	//写入文法检测信息
	int line;	//当前处理文件的行数
	string start;	//起始符
	set<string> nonTerminal;	//非终结符
	set<string> terminal;	//终结符
	map<string, vector<string>> production;	//产生式
	map<string, set<string>> first;	//非终结符first集合
	int stateNum = 0;	//状态集数目
	map<int, vector<pair<string, set<string>>>> states;	//状态集，pair里为生成式和向前看符号集
	map<int, map<string, int>> goTo;	//goto函数
	map<string, map<string, string>> action;	//action函数，即分析表

public:
	Parser()
	{
		line = 0;
	}
	Parser(string filePath)
	{
		line = 0;
		this->debug.open("./log/ParserTreeLog.txt", ios::out);
		this->file.open(filePath, ios::in);
		while (!file.is_open())
		{
			cout << "文法文件打开失败,请输入文件路径：" << endl;
			cin >> filePath;
			this->file.open(filePath, ios::in);
		}
		
		analyse();
		createFirstSet();
		makeClosure(0);
		createNewState(0);
		createTable();
	}
	~Parser()
	{
		file.close();
		debug.close();
	}
	//逐行解析文法
	void analyse();

	//将字符串 str 以 spt 分割为多个部分，储存在string容器中
	static vector<string> split(string str, string spt);

	//将符号串str切割为以非终结符或终结符顺序组成的集合
	vector<string> cut(string str);

	//判断是否为非终结符
	bool isNonTerminal(string s);

	//查找生成式 p 的first集合
	set<string> findFirst(string p);

	//构造文法的First集
	void createFirstSet();

	// 根据初始产生式，生成该状态的项目闭包
	vector<pair<string, set<string>>> makeClosure(int state);

	//根据传入的状态，生成从该状态可以转移的新状态集，并编写goto函数
	void createNewState(int state);

	//生成action函数
	void createTable();

	//根据文法产生的状态集，对token流进行文法分析
	bool parserSentence(vector<string> symbol);
};


