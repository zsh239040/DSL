#pragma once
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <set>
#include <vector>
#include <algorithm>
using namespace std;

//记录token的 id 和 属性
typedef struct token
{
	string id;
	string attribute;
}token;


class Lexical
{
private:
	int line;		//代码行数
	char ch;		//当前处理字符	
	int pos;		//当前处理字符所在位置
	string sentence;	//存储当前处理的字符串
	string currentToken;	//当前识别的token
	vector<token> tokenList;	//存储已识别的token

public:
	Lexical()
	{
		ch = '0';
		pos = 0;
		line = 0;
	}
	~Lexical()
	{
	}
	//识别一行字符串：str，转化为token
	vector<token> tokenize(string str);

	//获取下一个待识别字符
	char getChar();

	//撤回到上一个识别的字符
	void  retract();

	//处理行注释
	void lineComment();

	//识别标识符
	void identifier();

	//识别数字
	void digit();

	//处理字符串
	void strings();
};

