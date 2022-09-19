#pragma once
#include "Lexical.h"
#include "Parser.h"
#include "Script.h"
//#include "Errors.h"

extern vector<string> keyWord;

class ParserTree
{
private:
	Lexical lex;	//���ôʷ�������
	Parser *parser;	//�����ķ�������
	ifstream file;	//�򿪱�д�Ľű��ļ�
	Script &script;	//�ű��﷨��
	int stepNum;	//Step����
	string line;	//��ǰ�������
	int row;		//��ǰ�������к�

public:
	ParserTree(Script &script, string filePath):script(script)
	{
		row = 0;
		parser = new Parser("./data/grammar.txt");	//��ʼ���﷨������
		stepNum = -1;
		//steps = script.getSteps();

		this->file.open(filePath, ios::in);
		while (!file.is_open())
		{
			cout << "�����ļ���ʧ��,�������ļ�·����" << endl;
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
	//���н����ű��ļ�
	void parseLine();

	//�������ʷ��������ķ�������õ���token�������ݹؼ�����Ϣ�����﷨��
	void processTokens(vector<string> tokenList);

	//������Ϻ��ٴ��Եļ���﷨���Ƿ��걸
	void check();

	//���ؽű��﷨��
	Script& getScript();
};



