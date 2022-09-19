#include "Parser.h"

//���н����ķ�
void Parser::analyse()
{
	string temp;
	getline(file, temp);	//�ļ���һ�������������ս�������Կո����
	//�洢�ս��
	vector<string> terminals = split(temp, " ");
	for (auto& it : terminals)
		terminal.insert(it);

	while (getline(file, temp))
	{
		line++;
		string left, right;	//�洢�ķ��󲿷��ս�����Ҳ����Ŵ�
		int div = -1;
		for (size_t i = 0; i < temp.size() - 1; i++)
		{
			if (temp[i] == '-' && temp[i + 1] == '>')
			{
				div = i;
				break;
			}
			if (temp[i] != ' ')
				left += temp[i];
		}
		if (div == -1)
		{
			debug << "�Ƿ�����ʽ����ȷ�ϰ�����->������" << endl;
			return;
		}
		else if (left.size() > 1)
		{
			debug << "�Ƿ����ս������ȷ���󲿷��ս��ֻ����һ���ַ�" << endl;
			return;
		}
		else
		{
			for (size_t i = div + 2; i < temp.size(); i++)
				if (temp[i] != ' ')
					right += temp[i];
		}
		if (line == 1)	//�뱣֤��һ������ʽΪ��ʼ���Ĳ���ʽ
		{
			start = left + "\'";	//�ع��ķ�
			production[start].push_back(left);
			states[0].push_back({ start + "->." + left, { "$" } });
			nonTerminal.insert(start);
		}

		vector<string> elements = split(right, "|");

		//��ӻ���²���ʽ
		if (nonTerminal.find(left) != nonTerminal.end())
		{
			for (size_t j = 0; j < elements.size(); j++)
				production[left].push_back(elements[j]);
		}
		else
		{
			nonTerminal.insert(left);
			production[left] = elements;
		}
	}
}

//���ַ��� str �� spt �ָ�Ϊ������֣�������string������
vector<string> Parser::split(string str, string spt)
{
	vector<string> vec;
	size_t pos = 0;
	str += spt;	//ĩβ�ټ��Ϸָ��������ڷָ�
	while ((pos = str.find(spt, 0)) != string::npos)
	{
		string temp;
		for (size_t i = 0; i < pos; i++)
		{
			temp += str[i];
		}
		vec.emplace_back(temp);
		pos += spt.size();
		str.erase(0, pos);
	}
	return vec;
}

//�����Ŵ�str�и�Ϊ�Է��ս�����ս��˳����ɵļ���
vector<string> Parser::cut(string str)
{
	vector<string> res;
	if (str == "epsilon")
	{
		res.push_back(str);
		return res;
	}

	while (str.size())
	{
		int flag = 0;
		string::size_type pos;
		for (auto& it : terminal)
		{
			pos = str.find(it);
			if (pos == 0)
			{
				res.push_back(it);
				str.erase(pos, pos + it.size());
				flag = 1;
			}
		}
		for (auto& it : nonTerminal)
		{
			pos = str.find(it);
			if (pos == 0 && str[pos + it.size()] != '\'')
			{
				res.push_back(it);
				str.erase(pos, pos + it.size());
				flag = 1;
			}
		}
		if (!flag)
		{
			debug << "δ֪���ţ�����" << str[0] << endl;
			str.erase(0, 1);
		}
	}
	return res;
}

//�ж��Ƿ�Ϊ���ս��
bool Parser::isNonTerminal(string s)
{
	if (nonTerminal.find(s) != nonTerminal.end())
		return true;
	else
		return false;
}

//��������ʽ p ��first����
set<string> Parser::findFirst(string p)
{
	vector<string> symbol = cut(p);
	set<string> res;
	if (p == "")
		return res;
	if (p == "epsilon")
	{
		res.insert("epsilon");
		return res;
	}
	if (isNonTerminal(symbol[0])) // ���ַ�Ϊ���ս��ַ�����ݹ����first����
	{
		size_t i;
		for (i = 0; i < symbol.size(); i++)
		{
			int flag = false;	//epsilon �ݹ�����ı�־
			string N2 = symbol[i];
			if (isNonTerminal(N2))
			{
				for (auto& p2 : first[N2])
				{
					if (p2 == "epsilon")	//���������Եݹ����first��
						flag = true;
					else
						res.insert(p2);
				}
			}
			else
			{
				res.insert(N2);
				break;
			}
			if (!flag)
				break;
		}
		if (i == symbol.size())	//��ѯ��ĩβ��˵��������ʽ���¿�
			res.insert("epsilon");
	}
	else	//���ַ�Ϊ�ս��
		res.insert(symbol[0]);
	return res;
}

//�����ķ���First��
void Parser::createFirstSet()
{
	map<string, set<string>> new_first = first;
	do	//��������first����ֱ��first�����ٷ����仯
	{
		first = new_first;
		for (auto& N : nonTerminal)
		{
			for (auto& p : production[N]) // ĳ�����ս��ַ���ÿ������ʽ
			{
				set<string> res = findFirst(p);	//��ѯ�ò���ʽ��first��
				new_first[N].insert(res.begin(), res.end());
			}
		}
	} while (new_first != first);
}

// ���ݳ�ʼ����ʽ�����ɸ�״̬����Ŀ�հ�
vector<pair<string, set<string>>> Parser::makeClosure(int state)
{
	vector<pair<string, set<string>>> new_j = states[state];
	do
	{
		states[state] = new_j;
		for (auto& p : states[state])	//���ڸ�״̬��ÿһ������ʽ
		{
			string sentence = p.first;	//����ʽ
			set<string> symbol = p.second;	//��ǰ�����ż�
			int pos = sentence.find(".");	//��ѯ���жϷ��š�.����λ��
			if (pos != sentence.size() - 1)	//����.����ĩβ�����ǹ�Լʽ��
			{
				string temp;
				for (size_t i = pos + 1; i < sentence.size(); i++)
					temp.push_back(sentence[i]);
				vector<string> N = cut(temp);
				if (isNonTerminal(N[0]))	//'.'��ĵ�һ������Ϊ���ս��
				{
					string tail;
					for (size_t i = pos + 2; i < sentence.size(); i++)
						tail.push_back(sentence[i]);
					//��ѯ�ӡ�.����ĵڶ���λ�ÿ�ʼ���Ŵ���first��
					set<string> new_symbol = findFirst(tail);
					//��first����Ϊ�գ��������ǰ�����ţ�����̳�ԭ��ǰ������
					if (new_symbol.empty())
						new_symbol = symbol;

					for (auto& item : production[N[0]])
					{
						string new_sentence = N[0] + "->." + item;
						bool exist = false;
						for (auto& str : new_j)
						{
							if (new_sentence == str.first)//��ѯ�µĲ���ʽ�Ƿ�������Ŀ��
							{
								exist = true;
								str.second.insert(new_symbol.begin(), new_symbol.end());
								break;
							}
						}
						if (!exist)
							new_j.emplace_back(new_sentence, new_symbol);
					}
				}
			}
		}
	} while (new_j != states[state]);
	return new_j;
}

//���ݴ����״̬�����ɴӸ�״̬����ת�Ƶ���״̬��������дgoto����
void Parser::createNewState(int state)
{
	//���ڿ��������ظ���״̬����������ʱ���������ɵ�״̬��
	map<int, vector<pair<string, set<string>>>> temp_states;
	map<string, int> head;	//����ת�Ʒ��ź���һ״̬��״̬id
	int pre_num = stateNum;
	for (auto& item : states[state])
	{
		int pos = item.first.find('.');
		if (pos != item.first.size() - 1)	//�ǹ�Լʽ��
		{
			//�ҳ�ת�Ʒ��ţ�����Ԫ�أ�,����������'.'����
			string temp;
			for (size_t i = pos + 1; i < item.first.size(); i++)
				temp.push_back(item.first[i]);
			vector<string> symbol = cut(temp);
			string h = symbol[0];
			string new_sentence = item.first;
			new_sentence.erase(pos, 1);
			new_sentence.insert(pos + h.size(), ".");

			if (head.find(h) != head.end())	//��ת�Ʒ����Ѿ����ֹ���������״̬�����ɣ�����״̬�м��������ʽ
			{
				temp_states[head[h]].emplace_back(new_sentence, item.second);
			}
			else	//��ת�Ʒ���û�г��ֹ���������״̬
			{
				pre_num++;
				temp_states[pre_num].emplace_back(new_sentence, item.second);
				head[h] = pre_num;
			}
		}
	}
	pre_num = stateNum;
	for (auto& it : head)	//���ݴ����������״̬�����ɸ�״̬����Ŀ�������������ظ�����ɾ��
	{
		states[-1] = temp_states[it.second];
		vector<pair<string, set<string>>> temp = makeClosure(-1);
		int i;
		for (i = 0; i <= stateNum; i++) //�������ɵ�����״̬�ȶ�
		{
			if (states[i] == temp) //��Ŀ���Ѵ��ڣ�ɾ�������ɵ��ظ�״̬
			{
				states[it.second].clear();
				head[it.first] = i;
				break;
			}
		}
		if (i > stateNum) //���ڿ���ɾ���ظ�״̬��Ϊ��֤״̬���ֵ���������״̬���±��
		{
			states[++stateNum] = temp_states[it.second];
			it.second = stateNum;
			makeClosure(it.second);
		}
	}

	goTo[state] = head; //����goto����

	for (auto& it : head)	//�������ɵ�ÿһ��״̬���ݹ����ɸ�״̬��ת�Ƶ�״̬��
		if (it.second > pre_num)
			createNewState(it.second);
}

//����action����
void Parser::createTable()
{
	for (int i = 0; i <= stateNum; i++)
	{
		for (auto& terminal : terminal)
			action[to_string(i)][terminal] = "error";
		for (auto& N : nonTerminal)
			if (N != start)
				action[to_string(i)][N] = "error";
		action[to_string(i)]["$"] = "error";
	}

	for (int i = 0; i <= stateNum; i++)
	{
		for (auto& it : states[i])
		{
			int pos = it.first.find('.');
			if (pos != it.first.size() - 1)	//�ǹ�Լʽ��
			{
				string temp;
				for (size_t i = pos + 1; i < it.first.size(); i++) //�ҷ��� '.'���һ������
					temp.push_back(it.first[i]);
				vector<string> symbol = cut(temp);
				string head = symbol[0];
				if (!isNonTerminal(head))	//���ս������Ϊshift����
					action[to_string(i)][head] = "S" + to_string(goTo[i][head]);
				else
					action[to_string(i)][head] = to_string(goTo[i][head]);
			}
			else	//��Լʽ
			{
				vector<string> symbol = cut(split(it.first, "->")[0]); //����ʽ�󲿷���
				if (symbol[0] == start) //Ϊ��ʼ������������
					action[to_string(i)]["$"] = "ACC";
				else
				{
					for (auto& it2 : it.second) //������ǰ�����ţ������Լ����
					{
						string str = it.first;
						str.erase(str.size() - 1);
						action[to_string(i)][it2] = "R:" + str;
					}
				}
			}
		}
	}
}

//�����ķ�������״̬������token�������ķ�����
bool Parser::parserSentence(vector<string> tokens)
{
	bool accept = true;
	vector<string> Stack;
	vector<string> Input;
	Stack.push_back("0");
	Input.push_back("$");
	int Stack_top = 0;
	int Input_top = 0;
	for (int i = tokens.size() - 1; i >= 0; i--)
	{
		Input.push_back(tokens[i]);
		Input_top++;
	}

	debug << "��ʼ������";
	for (auto& item : tokens)
		debug << item << " ";
	debug << endl;
	while (!Stack.empty() && !Input.empty())
	{
		debug << "����ջ:	";
		for (auto& it : Stack)
			debug << it;
		debug << "	����ջ:	";
		for (auto& it : Input)
			debug << it;
		debug << "	�������:	";
		string act = action[Stack[Stack_top]][Input[Input_top]];
		if (act == "ACC")
		{
			debug << "��������" << endl<<endl;
			break;
		}
		else if (act == "error")
		{
			debug << "�﷨���󣡺���" << endl;
			accept = false;
			Input.pop_back();
			Input_top--;
		}
		else if (act[0] == 'S')
		{
			debug << "shift" << endl;
			string state = act.erase(0, 1);
			string ch = Input[Input_top];
			Input.pop_back();
			Input_top--;
			Stack.push_back(ch);
			Stack.push_back(state);
			Stack_top += 2;
		}
		else if (act[0] == 'R')
		{
			string sentence = act.erase(0, 2);
			debug << "reduce by " << sentence << endl;
			string N = split(sentence, "->")[0];
			vector<string> symbol = cut(split(sentence, "->")[1]);
			for (size_t i = 0; i < symbol.size(); i++)
			{
				Stack.pop_back();
				Stack.pop_back();
				Stack_top -= 2;
			}
			string new_state = action[Stack[Stack_top]][N];
			Stack.push_back(N);
			Stack.push_back(new_state);
			Stack_top += 2;
		}

	}
	if (accept)
		return true;
	else
		return false;
}