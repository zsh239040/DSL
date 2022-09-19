#include "Interpreter.h"
#include <conio.h>
#include <stdio.h>
#include <Windows.h>

//�����ű�
void Interpreter::intepret()
{
	vector<Step>& steps = script.getSteps();
	while (true) //����Step��洢������Ĳ����б� actionParms
	{
		//����ִ���е�Step
		Step &currentStep = steps[currentStepId];
		vector<string> expression = currentStep.getActionParms("Speak");
		debug(currentStep.getName() + ": ");

		//Backָ����Է��ص���һ��Step������interruptΪtrue�����Speak��ֱ�ӽ���Listen
		if (!interrupt && !expression.empty())
		{
			expressProc(expression);
		}
		if (interrupt)
		{
			interrupt = false;
		}

		//��������Ϊִ��ָ��
		if (currentStep.isCommandExist("Compute"))
			computeProc(currentStep.getActionParms("Compute"));

		if (currentStep.isCommandExist("Add"))
			addProc(currentStep.getActionParms("Add"));

		if (currentStep.isCommandExist("Delete"))
			delProc(currentStep.getActionParms("Delete"));

		
		//ͨ���û�answerȷ��������ת��Ŀ��
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
					debug(currentStep.getName()+ ": ����ȱ�١�Silence�����");
					cout << currentStep.getName() << ": ����ȱ�١�Silence�����" << endl;
				}
			}
			//answer��Ϊ�գ��鿴answer�Ƿ�ƥ��brunch��commands��������Ա���õ�brunch��
			else if (currentStep.isCommandExist(answer))
			{
				preStepId = currentStepId;
				currentStepId = currentStep.getTarget(answer);
				continue;
			}
			//ƥ��brunchʧ�ܣ����ű�������compare�����answer����Ϊ�ɱ����ݣ���Ҫ���бȽ�
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
			//���϶�ƥ��ʧ�ܣ������default
			else if(currentStep.isCommandExist("Default"))
			{
				preStepId = currentStepId;
				currentStepId = currentStep.getTarget("Default");
				continue;
			}
			else
			{
				debug(currentStep.getName() + "����ȱ�١�Default�����");
				cout << currentStep.getName() << ": ����ȱ�١�Default�����" << endl;
				return;
			}
		}
		//�����Stepû������listen�������default��ʵ��Ĭ����ת
		else if (currentStep.isCommandExist("Default"))
		{
			//interrupt = true;
			preStepId = currentStepId;
			currentStepId = currentStep.getTarget("Default");
			continue;
		}

		//��������ת��������һ��Step�������Speak
		if (currentStep.isCommandExist("Back"))
		{
			interrupt = true;
			currentStepId = preStepId;
			continue;
		}

		//��������ת���˳���
		if (currentStep.isCommandExist("Exit"))
		{
			return;
		}
	}
}

//��� expression ���
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
			//������ beginTime 
			if (_kbhit() && beginTime <= 0)
			{
				char ch = _getch();
				cout << ch;
				if (ch == 13)	//�����س�
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
		debug("Listen���������뱣֤��һ�������Ǹ����ڶ���������С��1");
		cout << "Listen���������뱣֤��һ�������Ǹ����ڶ���������С��1" << endl;
	}
	return sentence;
}

//�Ƚ��û��Ļش�answer���������parms����Ӧ���û����ݣ�ƥ��ɹ�������
bool Interpreter::compareProc(string answer,  vector<string> parms)
{
	bool isRight = false;
	debug("Compare  answer:" + answer + "  and  " + parms[1] + ": " + userInfo[parms[1]]);
	
	if (userInfo[parms[1]].find("|") != string::npos)
	{
		int pos = userInfo[parms[1]].find(answer);
		if (pos != userInfo[parms[1]].npos)
		{
			//answerӦΪ��ֵ�����е�ĳһ��ֵ���������Ƕ�ֵ������ĳ��ֵ��һ���֡�
			if (userInfo[parms[1]][pos + answer.size()] == '|')
				isRight = true;
		}
	}
	else if (userInfo[parms[1]] == answer)
		isRight = true;

	return isRight;
}

//parms��¼�������ʽ�����޼Ӽ�
void Interpreter::computeProc(vector<string> parms)
{
	float balance = atoi(userInfo[parms[1]].c_str());
	float deta;
	if (parms[3] == "&") //����Ϊ����
		deta = atoi(userInfo[parms[4]].c_str());
	else  //����Ϊ������
		deta = atoi(parms[3].c_str());
	char str[100];
	sprintf_s(str, 100, "%.2f", balance + deta);
	userInfo[parms[1]] = str;
	debug("Compute " + parms[1] + ": " + userInfo[parms[1]] + " + " + to_string(deta) + " = " + userInfo[parms[1]]);
}

//���û��ش�answer�����뵽parms��Ӧ���û�������
void Interpreter::addProc(vector<string> parms)
{
	debug("Add " + answer + " to " + parms[1] + ": " + userInfo[parms[1]]);
	if(userInfo[parms[1]].find("|") != userInfo[parms[1]].npos)
		userInfo[parms[1]] += answer + "|";
	else
	{
		cout << parms[1] << " ���Ƕ�ֵ���ԣ��޷�������Ӳ�����" << endl;
		debug(parms[1] + " ���Ƕ�ֵ���ԣ��޷�������Ӳ�����");
	}
}

//���û��ش��parms��Ӧ���û�������ȥ��
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
		cout << parms[1] << " ���Ƕ�ֵ���ԣ��޷�����ɾ��������" << endl;
		debug(parms[1] + " ���Ƕ�ֵ���ԣ��޷�����ɾ��������");
	}
}

//��ȡ�û��ش��еĹؼ���
string Interpreter::getKeywords(string answer, set<string> brunchs)
{
	for (auto& brunch : brunchs)
	{
		//ֻҪ�ɹ�ƥ�䵽һ���ؼ����򷵻عؼ���
		if (answer.find(brunch) != answer.npos)
			return brunch;
	}
	//ûƥ�䵽�򷵻�ԭ�ַ���
	return answer;
}

//����ʱ����д���û�����
void Interpreter::writeBack(User& user)
{
	user.setInfo(userInfo);
}

//��־��Ϣ
void Interpreter::debug(string info)
{
	SYSTEMTIME time;
	GetLocalTime(&time);
	logFile << time.wMonth << "-" << time.wDay << " " << time.wHour << ":" << time.wMinute << ":" << time.wSecond;
	logFile << "  " << info << endl;
}