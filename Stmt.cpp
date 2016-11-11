#include <iostream>
#include <stack>

#include "Stmt.h"

#define CONVERT(type, pointer) ((type *) pointer)

void FuncStmt::Print(std::ostream & os, int depth)
{
	os << std::string(depth, '|') << funcName << '(';

	for (auto name : args)
		os << name << ',';

	os << ')' << std::endl;
}

void ImpStmt::Print(std::ostream & os, int depth)
{
	os << std::string(depth, '|') << "Implication" << std::endl;

	left->Print(os, depth + 1);
	right->Print(os, depth + 1);
}

void OrStmt::Print(std::ostream & os, int depth)
{
	os << std::string(depth, '|') << "Or" << std::endl;

	left->Print(os, depth + 1);
	right->Print(os, depth + 1);
}

void AndStmt::Print(std::ostream & os, int depth)
{
	os << std::string(depth, '|') << "And" << std::endl;

	left->Print(os, depth + 1);
	right->Print(os, depth + 1);
}

void NotStmt::Print(std::ostream & os, int depth)
{
	os << std::string(depth, '|') << "Not" << std::endl;

	stmt->Print(os, depth + 1);
}

void OrMulStmt::Print(std::ostream & os, int depth)
{
	os << std::string(depth, '|') << "Or" << std::endl;

	for (auto stmt : stmts)
		stmt->Print(os, depth + 1);
}

void AndMulStmt::Print(std::ostream & os, int depth)
{
	os << std::string(depth, '|') << "And" << std::endl;

	for (auto stmt : stmts)
		stmt->Print(os, depth + 1);
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

Statement * Parser(const char * in, int & index)
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

			break;
		case IMP_REST:
			if (!resI)
				resI = resO;
			else
				((ImpStmt *)resI)->right = resO;

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
				((OrStmt*)resO)->right = resA;

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
				((AndStmt *)resA)->right = resN;
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

#undef CONVERT(type, pointer)