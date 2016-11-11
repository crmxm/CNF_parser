#include <stack>

#include "LogicTree.h"

#include <iostream>



void LogicTree::ReleaseAll(Statement * pS)
{
	if (pS->first)
		ReleaseAll(pS->first);
	if (pS->second)
		ReleaseAll(pS->second);

	delete pS;
}

Statement * LogicTree::ReduceImp(Statement * in)
{
	if (in->first)
		in->first = ReduceImp(in->first);
	if(in->second)
		in->second = ReduceImp(in->second);

	if (in->sType == Statement::StatementType::IMPLICATION)
	{
		Statement * sTemp = new OrStmt(new NotStmt(in->first), in->second);
		delete in;
		return sTemp;
	}
	else
		return in;
}

Statement * LogicTree::DistributeNot(Statement * in, bool isN)
{
	if (in->sType == Statement::StatementType::NOT)
		return DistributeNot(in->first, !isN);

	if (in->sType == Statement::StatementType::FUNC)
		if (isN)
			return new NotStmt(in);
		else
			return in;

	in->first = DistributeNot(in->first, isN);
	in->second = DistributeNot(in->second, isN);

	if (!isN)
		return in;

	Statement * sTemp;
	if (in->sType == Statement::StatementType::AND)
		sTemp = new OrStmt(in->first, in->second);
	else
		sTemp = new AndStmt(in->first, in->second);

	delete in;
	return sTemp;
}
/*
int LogicTree::EvaluateDisorderCount(Statement * in, int depth)
{
	if (in->sType == Statement::StatementType::NOT || in->sType == Statement::StatementType::FUNC)
		return 0;

	if (in->sType == Statement::StatementType::AND)
		return EvaluateDisorderCount(in->first, depth) + EvaluateDisorderCount(in->second, depth) + depth;
	else
		return EvaluateDisorderCount(in->first, depth + 1) + EvaluateDisorderCount(in->second, depth + 1);
}
*/
Statement * LogicTree::DistributeOrOnePass(Statement * in, bool & b)
{
	if (in->sType == Statement::StatementType::NOT || in->sType == Statement::StatementType::FUNC)
		return in;

	if (in->sType == Statement::StatementType::AND)
	{
		in->first = DistributeOrOnePass(in->first, b);
		in->second = DistributeOrOnePass(in->second, b);

		return in;
	}

	Statement * sTemp;
	Statement * first = DistributeOrOnePass(in->first, b);
	Statement * second = DistributeOrOnePass(in->second, b);
	bool isF = first->sType == Statement::StatementType::AND;
	bool isS = second->sType == Statement::StatementType::AND;

	if (isF && isS)
	{
		sTemp = new AndStmt(new AndStmt(new OrStmt(first->first, second->first),
										new OrStmt(first->first->DeepCopy(), second->second->DeepCopy())),
							new AndStmt(new OrStmt(first->second->DeepCopy(), second->first->DeepCopy()),
										new OrStmt(first->second, second->second)));
		delete first;
		delete second;
	}
	else if (isF)
	{
		sTemp = new AndStmt(new OrStmt(first->first, second), new OrStmt(first->second, second->DeepCopy()));
		delete first;
	}
	else if (isS)
	{
		sTemp = new AndStmt(new OrStmt(first, second->first), new OrStmt(first->DeepCopy(), second->second));
		delete second;
	}
	else
		sTemp = new OrStmt(first, second);

	delete in;

	b |= (isF | isS);

	return sTemp;
}
/*
Statement * LogicTree::CompressTree(Statement * in)
{
	if (in->sType == Statement::StatementType::FUNC || in->sType == Statement::StatementType::NOT)
		return in;

	Statement::StatementType nType = (in->sType > Statement::StatementType::NOT) ? in->sType :
		((in->sType == Statement::StatementType::OR) ? Statement::StatementType::OR_MUL : Statement::StatementType::AND_MUL);
	std::vector<Statement *> val;
	for (auto stmt : in->childs)
	{
		Statement * sTemp = CompressTree(stmt);

		if (sTemp->sType == nType)
		{
			val.insert(val.end(), sTemp->childs.begin(), sTemp->childs.end());
			delete sTemp;
		}
		else
			val.push_back(sTemp);
	}

	delete in;

	if (nType == Statement::StatementType::OR_MUL)
		return new OrMulStmt(val);
	else
		return new AndMulStmt(val);
}
*/
FuncStmt * ReadFunc(const char * in, int & index)
{
	int begin = index;
	while (in[index++] != '(');

	std::string name = std::string(in + begin, index - begin - 1);

	std::vector<std::string> val;

	while (true)
	{
		char c = '\0';
		begin = index;

		while ((c = in[index++]) != ',' && c != ')');

		val.push_back(std::string(in + begin, index - begin - 1));
		if (c == ')')
			break;
	}

	return new FuncStmt(name, val);
}

Statement * LogicTree::Parser(const char * in, int & index)
{
	enum ParserPhase
	{
		IMP,
		IMP_REST,
		OR,
		OR_REST,
		AND,
		AND_REST,
		NOT,
		FUNC,
	};
	std::stack<ParserPhase> sPhase;

	Statement * resI;
	Statement * resO;
	Statement * resA;
	Statement * resN;
	bool isNeg = false;
	resI = resO = resA = resN = nullptr;

	sPhase.push(ParserPhase::IMP);

	while (!sPhase.empty())
	{
		char cTemp = in[index];
		ParserPhase currPhase = sPhase.top();
		sPhase.pop();

		switch (currPhase)
		{
		case IMP:
			sPhase.push(ParserPhase::IMP_REST);
			sPhase.push(ParserPhase::OR);

			resI = nullptr;

			break;
		case OR:
			sPhase.push(ParserPhase::OR_REST);
			sPhase.push(ParserPhase::AND);

			resO = nullptr;

			break;
		case AND:
			sPhase.push(ParserPhase::AND_REST);
			sPhase.push(ParserPhase::NOT);

			resA = nullptr;

			break;
		case NOT:
			sPhase.push(ParserPhase::FUNC);

			if (cTemp == '~')
			{
				isNeg = true;
				index++;
			}
			resN = nullptr;

			break;
		case FUNC:
			if (cTemp == '(')
				resN = Parser(in, ++index);
			else
				resN = ReadFunc(in, index);

			if (isNeg)
				resN = new NotStmt(resN);

			isNeg = false;

			break;
		case IMP_REST:
			if (!resI)
				resI = resO;
			else
				resI->second = resO;
			//((ImpStmt *)resI)->right = resO;

			if (cTemp != '=')
			{
				if (cTemp == ')')
					index++;

				break;
			}

			index += 2;

			sPhase.push(ParserPhase::IMP_REST);
			sPhase.push(ParserPhase::OR);

			resI = new ImpStmt(resI);

			break;
		case OR_REST:
			if (!resO)
				resO = resA;
			else
				//((OrStmt*)resO)->right = resA;
				resO->second = resA;

			if (cTemp != '|')
				break;

			index++;

			sPhase.push(ParserPhase::OR_REST);
			sPhase.push(ParserPhase::AND);

			resO = new OrStmt(resO);

			break;
		case AND_REST:
			if (!resA)
				resA = resN;
			else
				//((AndStmt *)resA)->right = resN;
				resA->second = resN;
			if (cTemp != '&')
				break;

			index++;

			sPhase.push(ParserPhase::AND_REST);
			sPhase.push(ParserPhase::NOT);

			resA = new AndStmt(resA);

			break;
		}
	}

	return resI;
}

void LogicTree::DistributeOr()
{
	bool b = false;
	do
	{
		b = false;
		stmt = DistributeOrOnePass(stmt, b);
		//std::cout << "Distribute Or One Pass" << std::endl;
		//Print(std::cout);
	}
	while (b);

}