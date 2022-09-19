#include "ParserTree.h"
#include "Errors.h"

extern vector<Error> errors;
//将Step名转换为StepId
map<string, stepId> NameToID;	

//逐行解析脚本文件
void ParserTree::parseLine()
{
	while (getline(file, line))
	{
		row++;
		if (line.empty())	//跳过空行
			continue;
		
		vector<token> tokenList = lex.tokenize(line);	//词法解析，获得token流

		/*for (auto& it : tokenList)
			cout << it.id << " " << it.attribute << endl;
		cout << endl << endl;*/

		vector<string> symbols;
		vector<string> list;
		//将token的属性和标识分开，标识用于文法检查，属性用于生成语法树
		for (auto& it : tokenList)
		{
			symbols.push_back(it.id);
			list.push_back(it.attribute);
		}
		bool ret = parser->parserSentence(symbols);	//文法分析

		//该行语法分析成功，加入语法树
		if (ret)
			processTokens(list);
		else
		{
			cout << row << "行，语法错误\n" << endl;
			errors.push_back(Error("", "语法错误", row));
		}
		tokenList.clear();
	}
}

//处理经过词法分析和文法分析后得到的token流，根据关键字信息存入语法树
void ParserTree::processTokens(vector<string> tokenList)
{
	vector <Step> &steps = script.getSteps();
	if (steps.empty() && (tokenList[0]!="Start") && (tokenList[0] != "Step"))
		return;
	if (tokenList[0] == "Start")	//脚本入口
	{
		stepNum++;
		Step newStep(tokenList[1], stepNum);
		script.addStep(newStep);
		script.setEntry(stepNum);
		NameToID[tokenList[1]] = stepNum;
	}
	else if (tokenList[0] == "Step")	//脚本函数
	{
		stepNum++;
		Step newStep(tokenList[1], stepNum);
		script.addStep(newStep);
		NameToID[tokenList[1]] = stepNum;
	}
	else if (tokenList[0] == "Speak")	//客服陈述逻辑
	{
		tokenList.erase(tokenList.begin());
		steps[stepNum].addAction("Speak", tokenList);
	}
	else if (tokenList[0] == "Listen")	//聆听用户语句逻辑
	{
		steps[stepNum].addAction("Listen", { tokenList[1], tokenList[3] });
	}
	else if (tokenList[0] == "Brunch")	//关键词跳转分支逻辑
	{
		steps[stepNum].addTransition(tokenList[1], tokenList[3]);
		steps[stepNum].addBrunch(tokenList[1]);
	}
	else if (tokenList[0] == "Silence")	//用户沉默跳转逻辑
	{
		steps[stepNum].addTransition("Silence", tokenList[1]);
	}
	else if (tokenList[0] == "Default")	//默认跳转逻辑
	{
		steps[stepNum].addTransition("Default", tokenList[1]);
	}
	else if (tokenList[0] == "Compare")	//判断用户回答逻辑
	{
		tokenList.erase(tokenList.begin());
		steps[stepNum].addAction("Compare", tokenList);
	}
	else if (tokenList[0] == "Exit")	//脚本出口
	{
		script.setExit(stepNum);
		steps[stepNum].addCommand("Exit");
	}
	else if (tokenList[0] == "Back")	//返回到上一个Step执行，并且打断Speak，直接Listen
	{
		steps[stepNum].addCommand("Back");
	}
	else if (tokenList[0] == "Right")	//根据Compare的返回值True，做出状态跳转逻辑
	{
		steps[stepNum].addTransition("Right", tokenList[1]);
	}
	else if (tokenList[0] == "Wrong")	//根据Compare的返回值False，做出状态跳转逻辑
	{
		steps[stepNum].addTransition("Wrong", tokenList[1]);
	}
	else if (tokenList[0] == "Compute")	//计算表达式逻辑
	{
		tokenList.erase(tokenList.begin());
		steps[stepNum].addAction("Compute", tokenList);
	} 
	else if (tokenList[0] == "Add")	//将用户回答（answer）加入Add后的变量中
	{
		tokenList.erase(tokenList.begin());
		steps[stepNum].addAction("Add", tokenList);
	}
	else if (tokenList[0] == "Delete")//将用户回答（answer）从Delete后的变量中删除
	{
		tokenList.erase(tokenList.begin());
		steps[stepNum].addAction("Delete", tokenList);
	}
}

//解析完毕后再粗略的检查语法树是否完备
void ParserTree::check()
{
	for (auto& key : keyWord)
	{
		NameToID[key] = -1;
	}
	if (script.getEntry() < 0)
	{
		cout <<"未设置程序入口！"<<endl;
		errors.push_back(Error("", "未设置程序入口", 0));
	}
	if (script.getExit() < 0)
	{
		cout << "未设置程序出口！" << endl;
		errors.push_back(Error("", "未设置程序出口", 0));
	}
	vector <Step> steps = script.getSteps();
	for (auto& step : steps)
	{
		for (auto& transition : step.getTranstions())
		{
			if (NameToID.find(transition.second) == NameToID.end())
			{
				errors.push_back(Error(transition.second, "未定义此Step函数", 0));
				cout<< transition.second + ": 未定义此Step函数"<<endl;
			}
		}
	}
}

//返回脚本语法树
Script& ParserTree::getScript()
{
	return this->script;
}