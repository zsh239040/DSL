#include "Parser.h"

//逐行解析文法
void Parser::analyse()
{
	string temp;
	getline(file, temp);	//文件第一行请输入所有终结符，并以空格隔开
	//存储终结符
	vector<string> terminals = split(temp, " ");
	for (auto& it : terminals)
		terminal.insert(it);

	while (getline(file, temp))
	{
		line++;
		string left, right;	//存储文法左部非终结符和右部符号串
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
			debug << "非法产生式！请确认包含‘->’符号" << endl;
			return;
		}
		else if (left.size() > 1)
		{
			debug << "非法非终结符！请确认左部非终结符只含有一个字符" << endl;
			return;
		}
		else
		{
			for (size_t i = div + 2; i < temp.size(); i++)
				if (temp[i] != ' ')
					right += temp[i];
		}
		if (line == 1)	//请保证第一个产生式为起始符的产生式
		{
			start = left + "\'";	//拓广文法
			production[start].push_back(left);
			states[0].push_back({ start + "->." + left, { "$" } });
			nonTerminal.insert(start);
		}

		vector<string> elements = split(right, "|");

		//添加或更新产生式
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

//将字符串 str 以 spt 分割为多个部分，储存在string容器中
vector<string> Parser::split(string str, string spt)
{
	vector<string> vec;
	size_t pos = 0;
	str += spt;	//末尾再加上分隔符，便于分割
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

//将符号串str切割为以非终结符或终结符顺序组成的集合
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
			debug << "未知符号！跳过" << str[0] << endl;
			str.erase(0, 1);
		}
	}
	return res;
}

//判断是否为非终结符
bool Parser::isNonTerminal(string s)
{
	if (nonTerminal.find(s) != nonTerminal.end())
		return true;
	else
		return false;
}

//查找生成式 p 的first集合
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
	if (isNonTerminal(symbol[0])) // 首字符为非终结字符，则递归查找first集合
	{
		size_t i;
		for (i = 0; i < symbol.size(); i++)
		{
			int flag = false;	//epsilon 递归结束的标志
			string N2 = symbol[i];
			if (isNonTerminal(N2))
			{
				for (auto& p2 : first[N2])
				{
					if (p2 == "epsilon")	//表明还可以递归查找first集
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
		if (i == symbol.size())	//查询到末尾，说明该生成式可致空
			res.insert("epsilon");
	}
	else	//首字符为终结符
		res.insert(symbol[0]);
	return res;
}

//构造文法的First集
void Parser::createFirstSet()
{
	map<string, set<string>> new_first = first;
	do	//不断生成first集，直到first集不再发生变化
	{
		first = new_first;
		for (auto& N : nonTerminal)
		{
			for (auto& p : production[N]) // 某个非终结字符的每个生成式
			{
				set<string> res = findFirst(p);	//查询该产生式的first集
				new_first[N].insert(res.begin(), res.end());
			}
		}
	} while (new_first != first);
}

// 根据初始产生式，生成该状态的项目闭包
vector<pair<string, set<string>>> Parser::makeClosure(int state)
{
	vector<pair<string, set<string>>> new_j = states[state];
	do
	{
		states[state] = new_j;
		for (auto& p : states[state])	//对于该状态的每一个产生式
		{
			string sentence = p.first;	//产生式
			set<string> symbol = p.second;	//向前看符号集
			int pos = sentence.find(".");	//查询并判断符号‘.’的位置
			if (pos != sentence.size() - 1)	//符号.不在末尾，即非归约式子
			{
				string temp;
				for (size_t i = pos + 1; i < sentence.size(); i++)
					temp.push_back(sentence[i]);
				vector<string> N = cut(temp);
				if (isNonTerminal(N[0]))	//'.'后的第一个符号为非终结符
				{
					string tail;
					for (size_t i = pos + 2; i < sentence.size(); i++)
						tail.push_back(sentence[i]);
					//查询从‘.’后的第二个位置开始符号串的first集
					set<string> new_symbol = findFirst(tail);
					//若first集不为空，则更新向前看符号，否则继承原向前看符号
					if (new_symbol.empty())
						new_symbol = symbol;

					for (auto& item : production[N[0]])
					{
						string new_sentence = N[0] + "->." + item;
						bool exist = false;
						for (auto& str : new_j)
						{
							if (new_sentence == str.first)//查询新的产生式是否已在项目中
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

//根据传入的状态，生成从该状态可以转移的新状态集，并编写goto函数
void Parser::createNewState(int state)
{
	//由于可能生成重复的状态集，故先临时储存新生成的状态集
	map<int, vector<pair<string, set<string>>>> temp_states;
	map<string, int> head;	//储存转移符号和下一状态的状态id
	int pre_num = stateNum;
	for (auto& item : states[state])
	{
		int pos = item.first.find('.');
		if (pos != item.first.size() - 1)	//非规约式子
		{
			//找出转移符号（即首元素）,并将将符号'.'右移
			string temp;
			for (size_t i = pos + 1; i < item.first.size(); i++)
				temp.push_back(item.first[i]);
			vector<string> symbol = cut(temp);
			string h = symbol[0];
			string new_sentence = item.first;
			new_sentence.erase(pos, 1);
			new_sentence.insert(pos + h.size(), ".");

			if (head.find(h) != head.end())	//该转移符号已经出现过，表明新状态已生成，在新状态中加入该生成式
			{
				temp_states[head[h]].emplace_back(new_sentence, item.second);
			}
			else	//该转移符号没有出现过，生成新状态
			{
				pre_num++;
				temp_states[pre_num].emplace_back(new_sentence, item.second);
				head[h] = pre_num;
			}
		}
	}
	pre_num = stateNum;
	for (auto& it : head)	//根据储存的所有新状态，生成该状态的项目集，若与其他重复，则删除
	{
		states[-1] = temp_states[it.second];
		vector<pair<string, set<string>>> temp = makeClosure(-1);
		int i;
		for (i = 0; i <= stateNum; i++) //与已生成的所有状态比对
		{
			if (states[i] == temp) //项目集已存在，删除新生成的重复状态
			{
				states[it.second].clear();
				head[it.first] = i;
				break;
			}
		}
		if (i > stateNum) //由于可能删除重复状态，为保证状态数字的连续，给状态重新编号
		{
			states[++stateNum] = temp_states[it.second];
			it.second = stateNum;
			makeClosure(it.second);
		}
	}

	goTo[state] = head; //更新goto函数

	for (auto& it : head)	//对新生成的每一个状态，递归生成该状态可转移的状态集
		if (it.second > pre_num)
			createNewState(it.second);
}

//生成action函数
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
			if (pos != it.first.size() - 1)	//非规约式子
			{
				string temp;
				for (size_t i = pos + 1; i < it.first.size(); i++) //找符号 '.'后的一个符号
					temp.push_back(it.first[i]);
				vector<string> symbol = cut(temp);
				string head = symbol[0];
				if (!isNonTerminal(head))	//是终结符，则为shift函数
					action[to_string(i)][head] = "S" + to_string(goTo[i][head]);
				else
					action[to_string(i)][head] = to_string(goTo[i][head]);
			}
			else	//规约式
			{
				vector<string> symbol = cut(split(it.first, "->")[0]); //产生式左部符号
				if (symbol[0] == start) //为起始符，则分析完成
					action[to_string(i)]["$"] = "ACC";
				else
				{
					for (auto& it2 : it.second) //根据向前看符号，构造归约函数
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

//根据文法产生的状态集，对token流进行文法分析
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

	debug << "开始分析：";
	for (auto& item : tokens)
		debug << item << " ";
	debug << endl;
	while (!Stack.empty() && !Input.empty())
	{
		debug << "符号栈:	";
		for (auto& it : Stack)
			debug << it;
		debug << "	输入栈:	";
		for (auto& it : Input)
			debug << it;
		debug << "	分析语句:	";
		string act = action[Stack[Stack_top]][Input[Input_top]];
		if (act == "ACC")
		{
			debug << "分析结束" << endl<<endl;
			break;
		}
		else if (act == "error")
		{
			debug << "语法错误！忽略" << endl;
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