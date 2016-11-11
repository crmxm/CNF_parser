#include <iostream>
#include <stack>

#include "Stmt.h"

const char StatementTypeName[7][4] = { "", "Imp", "Or", "And", "Not", "Or", "And" };

void Statement::Print(std::ostream & os, int depth)
{
	os << std::string(depth, '|') << StatementTypeName[sType] << std::endl;

	for (auto stmt : childs)
		stmt->Print(os, depth + 1);
}

void FuncStmt::Print(std::ostream & os, int depth)
{
	os << std::string(depth, '|') << funcName << '(';

	for (auto name : args)
		os << name << ',';

	os << ')' << std::endl;
}

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

void LogicTree::ReleaseAll(Statement * pS)
{
	for (auto stmt : pS->childs)
		ReleaseAll(stmt);

	delete pS;
}

Statement * LogicTree::ReduceImp(Statement * in)
{
	std::vector<Statement *> & rvS = in->childs;

	for (unsigned int i = 0; i < rvS.size(); i++)
		rvS[i] = ReduceImp(rvS[i]);
	
	if (in->sType == Statement::StatementType::IMPLICATION)
	{
		Statement * sTemp = new OrStmt(new NotStmt(rvS[0]), rvS[1]);
		delete in;
		return sTemp;
	}
	else
		return in;
}

Statement * LogicTree::DistributeNot(Statement * in, bool isN)
{
	if (in->sType == Statement::StatementType::NOT)
		return DistributeNot(in->childs[0], !isN);

	if (in->sType == Statement::StatementType::FUNC && isN)
		return new NotStmt(in);

	std::vector<Statement *> & rvS = in->childs;

	for (unsigned int i = 0; i < rvS.size(); i++)
		rvS[i] = DistributeNot(rvS[i], isN);

	Statement * sTemp;
	if (!isN)
		return in;
	if (in->sType == Statement::StatementType::AND)
		sTemp = new OrStmt(rvS[0], rvS[1]);
	else
		sTemp = new AndStmt(rvS[0], rvS[1]);
	
	delete in;
	return sTemp;
}

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
				resI->AddChild(resO);
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
				resO->AddChild(resA);

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
				resA->AddChild(resN);
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
