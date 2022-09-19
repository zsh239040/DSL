#pragma once
#include "Lexical.h"
#include "Parser.h"
#include "Script.h"
//#include "Errors.h"

extern vector<string> keyWord;

class ParserTree
{
private:
	Lexical lex;	//调用词法解析器
	Parser *parser;	//调用文法解析器
	ifstream file;	//打开编写的脚本文件
	Script &script;	//脚本语法树
	int stepNum;	//Step数量
	string line;	//当前解析语句
	int row;		//当前解析的行号

public:
	ParserTree(Script &script, string filePath):script(script)
	{
		row = 0;
		parser = new Parser("./data/grammar.txt");	//初始化语法解析器
		stepNum = -1;
		//steps = script.getSteps();

		this->file.open(filePath, ios::in);
		while (!file.is_open())
		{
			cout << "程序文件打开失败,请输入文件路径：" << endl;
			cin >> filePath;
			this->file.open(filePath, ios::in);
		}
		parseLine();
		check();
		lex.~Lexical();
		delete parser;
	}
	~ParserTree()
	{
		file.close();
	}
	//逐行解析脚本文件
	void parseLine();

	//处理经过词法分析和文法分析后得到的token流，根据关键字信息存入语法树
	void processTokens(vector<string> tokenList);

	//解析完毕后再粗略的检查语法树是否完备
	void check();

	//返回脚本语法树
	Script& getScript();
};



