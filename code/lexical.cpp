#include "Lexical.h"
#include "Errors.h"

extern vector<Error>errors;

//�ؼ��ֱ�
vector<string> keyWord = {
"Start","Step","Listen","Default","Speak","Home", "Back",
"Brunch","Compare","Compute","Add","Delete","Wrong","Right","Silence","Exit" };

//ʶ��һ���ַ�����str��ת��Ϊtoken
vector<token> Lexical::tokenize(string str)
{
	str += '\n';
	this->sentence = str;	
	currentToken.clear();
	tokenList.clear();
	//���ַ������ַ���
	for(pos = 0; pos < sentence.size(); )
	{
		ch = getChar();
		//����ע��
		if (ch == '#')
			lineComment();

		//ʶ���ʶ��������ĸ���»��߿�ͷ�������ַ���
		else if (isalpha(ch) || ch == '_')
		{
			identifier();
			currentToken.clear();
		}

		//ʶ�����֣���С��������ֿ�ͷ�����ִ�
		else if (isdigit(ch) || ch == '.')
		{
			digit();
			currentToken.clear();
		}

		//ʶ���ַ���
		else if (ch == '"')
		{
			strings();
			currentToken.clear();
		}

		//ʶ����������
		else if (ch == ',' || ch == '+' || ch == '-' || ch == '*' || ch == '&')
		{
			tokenList.push_back({ string(1,ch), string(1,ch) });
		}
		
		else if (ch == ' ' || ch == '\t' || ch == '\n')
		{
		}

		else
		{
			errors.push_back({ currentToken, "�޷�ʶ���ַ���", line });
		}
	}
	return tokenList;
}

//��ȡ��һ����ʶ���ַ�
char Lexical::getChar()
{
	char c = sentence[pos++];
	if (c == '\n') 
	{
		line++;
	}
		
	return c;
}

//���ص���һ��ʶ����ַ�
void  Lexical::retract()
{
	char c = sentence[pos--];
	if (c == '\n')
		line--;
}

//������ע��
void Lexical::lineComment()
{
	while (getChar() != '\n')
		getChar();
}

//ʶ���ʶ��
void Lexical::identifier()
{
	while (isalpha(ch) || isdigit(ch) || ch == '_')
	{
		currentToken += ch;
		ch = getChar();
	}
	retract();
	vector<string>::iterator res = find(keyWord.begin(), keyWord.end(), currentToken);
	//��ʶ��Ϊ�ؼ���
	if (res != keyWord.end())
		tokenList.push_back({ currentToken, currentToken });
	else
	{
		tokenList.push_back({ "id", currentToken });
	}
}

//ʶ������
void Lexical::digit()
{
	currentToken += ch;
	int state = 6;		//��ʼ״̬

	int stateTransfer[9][5] =	//״̬ת�Ʊ�
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
		//E �� e Ϊָ����־
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
	if (state == -1)	//ʶ�����
	{
		while (isalpha(ch) || isdigit(ch) || ch == '_' || ch == '.')	//���������ַ���
		{
			currentToken += ch;
			ch = getChar();
		}
		errors.push_back({ currentToken, "�Ƿ����ֻ��ʶ��", line });
		return;
	}
	else
	{
		tokenList.push_back({ "num", currentToken });
		retract();
	}
}

//�����ַ���
void Lexical::strings()
{
	while ((ch = getChar()) != '"')
	{
		if (ch == '\\')		//��ֹ��'\"'�е�˫��������Ϊ�ַ����Ľ�����־
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
			errors.push_back({ currentToken, "ȱʧ��˫����", line });
			return;
		}
		currentToken += ch;
	}
	tokenList.push_back({ "str", currentToken });
}
