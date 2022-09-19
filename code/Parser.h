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
	fstream file;	//����Ƶ��ķ��ļ�
	fstream debug;	//д���ķ������Ϣ
	int line;	//��ǰ�����ļ�������
	string start;	//��ʼ��
	set<string> nonTerminal;	//���ս��
	set<string> terminal;	//�ս��
	map<string, vector<string>> production;	//����ʽ
	map<string, set<string>> first;	//���ս��first����
	int stateNum = 0;	//״̬����Ŀ
	map<int, vector<pair<string, set<string>>>> states;	//״̬����pair��Ϊ����ʽ����ǰ�����ż�
	map<int, map<string, int>> goTo;	//goto����
	map<string, map<string, string>> action;	//action��������������

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
			cout << "�ķ��ļ���ʧ��,�������ļ�·����" << endl;
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
	//���н����ķ�
	void analyse();

	//���ַ��� str �� spt �ָ�Ϊ������֣�������string������
	static vector<string> split(string str, string spt);

	//�����Ŵ�str�и�Ϊ�Է��ս�����ս��˳����ɵļ���
	vector<string> cut(string str);

	//�ж��Ƿ�Ϊ���ս��
	bool isNonTerminal(string s);

	//��������ʽ p ��first����
	set<string> findFirst(string p);

	//�����ķ���First��
	void createFirstSet();

	// ���ݳ�ʼ����ʽ�����ɸ�״̬����Ŀ�հ�
	vector<pair<string, set<string>>> makeClosure(int state);

	//���ݴ����״̬�����ɴӸ�״̬����ת�Ƶ���״̬��������дgoto����
	void createNewState(int state);

	//����action����
	void createTable();

	//�����ķ�������״̬������token�������ķ�����
	bool parserSentence(vector<string> symbol);
};


