#include "Lexical.h"
#include "Errors.h"

extern vector<Error>errors;

//关键字表
vector<string> keyWord = {
"Start","Step","Listen","Default","Speak","Home", "Back",
"Brunch","Compare","Compute","Add","Delete","Wrong","Right","Silence","Exit" };

//识别一行字符串：str，转化为token
vector<token> Lexical::tokenize(string str)
{
	str += '\n';
	this->sentence = str;	
	currentToken.clear();
	tokenList.clear();
	//逐字符解析字符串
	for(pos = 0; pos < sentence.size(); )
	{
		ch = getChar();
		//忽略注释
		if (ch == '#')
			lineComment();

		//识别标识符：以字母和下划线开头的所有字符串
		else if (isalpha(ch) || ch == '_')
		{
			identifier();
			currentToken.clear();
		}

		//识别数字：以小数点或数字开头的数字串
		else if (isdigit(ch) || ch == '.')
		{
			digit();
			currentToken.clear();
		}

		//识别字符串
		else if (ch == '"')
		{
			strings();
			currentToken.clear();
		}

		//识别其他符号
		else if (ch == ',' || ch == '+' || ch == '-' || ch == '*' || ch == '&')
		{
			tokenList.push_back({ string(1,ch), string(1,ch) });
		}
		
		else if (ch == ' ' || ch == '\t' || ch == '\n')
		{
		}

		else
		{
			errors.push_back({ currentToken, "无法识别字符串", line });
		}
	}
	return tokenList;
}

//获取下一个待识别字符
char Lexical::getChar()
{
	char c = sentence[pos++];
	if (c == '\n') 
	{
		line++;
	}
		
	return c;
}

//撤回到上一个识别的字符
void  Lexical::retract()
{
	char c = sentence[pos--];
	if (c == '\n')
		line--;
}

//处理行注释
void Lexical::lineComment()
{
	while (getChar() != '\n')
		getChar();
}

//识别标识符
void Lexical::identifier()
{
	while (isalpha(ch) || isdigit(ch) || ch == '_')
	{
		currentToken += ch;
		ch = getChar();
	}
	retract();
	vector<string>::iterator res = find(keyWord.begin(), keyWord.end(), currentToken);
	//标识符为关键字
	if (res != keyWord.end())
		tokenList.push_back({ currentToken, currentToken });
	else
	{
		tokenList.push_back({ "id", currentToken });
	}
}

//识别数字
void Lexical::digit()
{
	currentToken += ch;
	int state = 6;		//初始状态

	int stateTransfer[9][5] =	//状态转移表
	{
				{ 0, 1, 6, 2,-1},
				{-1,-1, 6, 2,-1},
				{-1,-1, 3,-1,-1},
				{ 8,-1, 3,-1, 4},
				{-1, 7, 5,-1,-1},
				{ 8,-1, 5,-1,-1},
				{ 8,-1, 6, 3, 4},
				{-1,-1, 5,-1,-1},
				{ 8,-1,-1,-1,-1}
	};
	while (state != -1 && state != 8)
	{
		ch = getChar();
		//E 和 e 为指数标志
		if (isalpha(ch) && ch != 'e' && ch != 'E')
			state = -1;
		else if (ch == '+' || ch == '-')
			state = stateTransfer[state][1];
		else if (ch >= '0' && ch <= '9')
			state = stateTransfer[state][2];
		else if (ch == '.')
			state = stateTransfer[state][3];
		else if (ch == 'e' || ch == 'E')
			state = stateTransfer[state][4];
		else
			state = stateTransfer[state][0];

		if (state != -1 && state != 8)
			currentToken += ch;
	}
	if (state == -1)	//识别出错
	{
		while (isalpha(ch) || isdigit(ch) || ch == '_' || ch == '.')	//跳过错误字符串
		{
			currentToken += ch;
			ch = getChar();
		}
		errors.push_back({ currentToken, "非法数字或标识符", line });
		return;
	}
	else
	{
		tokenList.push_back({ "num", currentToken });
		retract();
	}
}

//处理字符串
void Lexical::strings()
{
	while ((ch = getChar()) != '"')
	{
		if (ch == '\\')		//防止将'\"'中的双引号误认为字符串的结束标志
		{
			char nextChar = getChar();
			if (nextChar == '"')
			{
				currentToken += "\\\"";
				continue;
			}
			else
				retract();
		}
		if (ch == '\n')
		{
			retract();
			errors.push_back({ currentToken, "缺失右双引号", line });
			return;
		}
		currentToken += ch;
	}
	tokenList.push_back({ "str", currentToken });
}
