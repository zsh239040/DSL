#include "ParserTree.h"
#include "Errors.h"

extern vector<Error> errors;
//��Step��ת��ΪStepId
map<string, stepId> NameToID;	

//���н����ű��ļ�
void ParserTree::parseLine()
{
	while (getline(file, line))
	{
		row++;
		if (line.empty())	//��������
			continue;
		
		vector<token> tokenList = lex.tokenize(line);	//�ʷ����������token��

		/*for (auto& it : tokenList)
			cout << it.id << " " << it.attribute << endl;
		cout << endl << endl;*/

		vector<string> symbols;
		vector<string> list;
		//��token�����Ժͱ�ʶ�ֿ�����ʶ�����ķ���飬�������������﷨��
		for (auto& it : tokenList)
		{
			symbols.push_back(it.id);
			list.push_back(it.attribute);
		}
		bool ret = parser->parserSentence(symbols);	//�ķ�����

		//�����﷨�����ɹ��������﷨��
		if (ret)
			processTokens(list);
		else
		{
			cout << row << "�У��﷨����\n" << endl;
			errors.push_back(Error("", "�﷨����", row));
		}
		tokenList.clear();
	}
}

//�������ʷ��������ķ�������õ���token�������ݹؼ�����Ϣ�����﷨��
void ParserTree::processTokens(vector<string> tokenList)
{
	vector <Step> &steps = script.getSteps();
	if (steps.empty() && (tokenList[0]!="Start") && (tokenList[0] != "Step"))
		return;
	if (tokenList[0] == "Start")	//�ű����
	{
		stepNum++;
		Step newStep(tokenList[1], stepNum);
		script.addStep(newStep);
		script.setEntry(stepNum);
		NameToID[tokenList[1]] = stepNum;
	}
	else if (tokenList[0] == "Step")	//�ű�����
	{
		stepNum++;
		Step newStep(tokenList[1], stepNum);
		script.addStep(newStep);
		NameToID[tokenList[1]] = stepNum;
	}
	else if (tokenList[0] == "Speak")	//�ͷ������߼�
	{
		tokenList.erase(tokenList.begin());
		steps[stepNum].addAction("Speak", tokenList);
	}
	else if (tokenList[0] == "Listen")	//�����û�����߼�
	{
		steps[stepNum].addAction("Listen", { tokenList[1], tokenList[3] });
	}
	else if (tokenList[0] == "Brunch")	//�ؼ�����ת��֧�߼�
	{
		steps[stepNum].addTransition(tokenList[1], tokenList[3]);
		steps[stepNum].addBrunch(tokenList[1]);
	}
	else if (tokenList[0] == "Silence")	//�û���Ĭ��ת�߼�
	{
		steps[stepNum].addTransition("Silence", tokenList[1]);
	}
	else if (tokenList[0] == "Default")	//Ĭ����ת�߼�
	{
		steps[stepNum].addTransition("Default", tokenList[1]);
	}
	else if (tokenList[0] == "Compare")	//�ж��û��ش��߼�
	{
		tokenList.erase(tokenList.begin());
		steps[stepNum].addAction("Compare", tokenList);
	}
	else if (tokenList[0] == "Exit")	//�ű�����
	{
		script.setExit(stepNum);
		steps[stepNum].addCommand("Exit");
	}
	else if (tokenList[0] == "Back")	//���ص���һ��Stepִ�У����Ҵ��Speak��ֱ��Listen
	{
		steps[stepNum].addCommand("Back");
	}
	else if (tokenList[0] == "Right")	//����Compare�ķ���ֵTrue������״̬��ת�߼�
	{
		steps[stepNum].addTransition("Right", tokenList[1]);
	}
	else if (tokenList[0] == "Wrong")	//����Compare�ķ���ֵFalse������״̬��ת�߼�
	{
		steps[stepNum].addTransition("Wrong", tokenList[1]);
	}
	else if (tokenList[0] == "Compute")	//������ʽ�߼�
	{
		tokenList.erase(tokenList.begin());
		steps[stepNum].addAction("Compute", tokenList);
	} 
	else if (tokenList[0] == "Add")	//���û��ش�answer������Add��ı�����
	{
		tokenList.erase(tokenList.begin());
		steps[stepNum].addAction("Add", tokenList);
	}
	else if (tokenList[0] == "Delete")//���û��ش�answer����Delete��ı�����ɾ��
	{
		tokenList.erase(tokenList.begin());
		steps[stepNum].addAction("Delete", tokenList);
	}
}

//������Ϻ��ٴ��Եļ���﷨���Ƿ��걸
void ParserTree::check()
{
	for (auto& key : keyWord)
	{
		NameToID[key] = -1;
	}
	if (script.getEntry() < 0)
	{
		cout <<"δ���ó�����ڣ�"<<endl;
		errors.push_back(Error("", "δ���ó������", 0));
	}
	if (script.getExit() < 0)
	{
		cout << "δ���ó�����ڣ�" << endl;
		errors.push_back(Error("", "δ���ó������", 0));
	}
	vector <Step> steps = script.getSteps();
	for (auto& step : steps)
	{
		for (auto& transition : step.getTranstions())
		{
			if (NameToID.find(transition.second) == NameToID.end())
			{
				errors.push_back(Error(transition.second, "δ�����Step����", 0));
				cout<< transition.second + ": δ�����Step����"<<endl;
			}
		}
	}
}

//���ؽű��﷨��
Script& ParserTree::getScript()
{
	return this->script;
}