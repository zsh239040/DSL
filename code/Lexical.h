#pragma once
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <set>
#include <vector>
#include <algorithm>
using namespace std;

//��¼token�� id �� ����
typedef struct token
{
	string id;
	string attribute;
}token;


class Lexical
{
private:
	int line;		//��������
	char ch;		//��ǰ�����ַ�	
	int pos;		//��ǰ�����ַ�����λ��
	string sentence;	//�洢��ǰ������ַ���
	string currentToken;	//��ǰʶ���token
	vector<token> tokenList;	//�洢��ʶ���token

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
	//ʶ��һ���ַ�����str��ת��Ϊtoken
	vector<token> tokenize(string str);

	//��ȡ��һ����ʶ���ַ�
	char getChar();

	//���ص���һ��ʶ����ַ�
	void  retract();

	//������ע��
	void lineComment();

	//ʶ���ʶ��
	void identifier();

	//ʶ������
	void digit();

	//�����ַ���
	void strings();
};

