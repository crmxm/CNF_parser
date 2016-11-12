#include <iostream>

#include "Stmt.h"

const char StatementTypeName[5][4] = { "", "Imp", "Or", "And", "Not" };

void Statement::Print(std::ostream & os, int depth) const
{
	os << std::string(depth, '|') << StatementTypeName[sType] << std::endl;

	first->Print(os, depth + 1);

	if (second)
		second->Print(os, depth + 1);
}

Statement * Statement::ReduceImp()
{
	Statement * temp;
	if (temp = first)
		if (temp != (first = first->ReduceImp()))
			delete temp;

	if (temp = second)
		if (temp != (second = second->ReduceImp()))
			delete temp;

	return this;
}

Statement * Statement::DistributeNot(bool isN)
{
	Statement * temp = first;
	if (temp != (first = first->DistributeNot(isN)))
		delete temp;

	temp = second;
	if (temp != (second = second->DistributeNot(isN)))
		delete temp;

	if (!isN)
		return this;

	if (sType == StatementType::OR)
		return new AndStmt(first, second);
	else
		return new OrStmt(first, second);
}

Statement * Statement::DistributeOrOnePass(bool & hasChange)
{
	Statement * temp = first;
	if (temp != (first = first->DistributeOrOnePass(hasChange)))
		delete temp;

	temp = second;
	if (temp != (second = second->DistributeOrOnePass(hasChange)))
		delete temp;

	return this;
}

void FuncStmt::Print(std::ostream & os, int depth) const
{
	os << std::string(depth, '|') << funcName << '(';

	for (auto name : args)
		os << name << ',';

	os << ')' << std::endl;
}

Statement * FuncStmt::DeepCopy() const
{
	return new FuncStmt(funcName, args);
}

Statement * FuncStmt::DistributeNot(bool isN)
{
	return (isN) ? new NotStmt(new FuncStmt(*this)) : (Statement *) this;
}

Statement * ImpStmt::DeepCopy() const
{
	return new ImpStmt(first->DeepCopy(), second->DeepCopy());
}

Statement * ImpStmt::ReduceImp()
{
	return new OrStmt(new NotStmt(first->ReduceImp()), second->ReduceImp());
}

Statement * OrStmt::DeepCopy() const 
{
	return new OrStmt(first->DeepCopy(), second->DeepCopy());
}

Statement * OrStmt::DistributeOrOnePass(bool & hasChange)
{
	Statement::DistributeOrOnePass(hasChange);

	bool isF = first->sType == StatementType::AND;
	bool isS = second->sType == StatementType::AND;

	Statement * sTemp;
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
		sTemp = this;
	
	hasChange |= (isF | isS);
	return sTemp;
}

Statement * AndStmt::DeepCopy() const
{
	return new AndStmt(first->DeepCopy(), second->DeepCopy());
}

Statement * AndStmt::DistributeOrOnePass(bool & hasChange)
{
	return Statement::DistributeOrOnePass(hasChange);
}

Statement * NotStmt::DeepCopy() const
{
	return new NotStmt(first->DeepCopy());
}

Statement * NotStmt::DistributeNot(bool isN)
{
	Statement * temp = first;
	if (temp != (first = first->DistributeNot(!isN)))
		delete temp;

	return first;
}

void ReleaseAll(Statement * pS)
{
	if (pS->first)
		ReleaseAll(pS->first);
	if (pS->second)
		ReleaseAll(pS->second);

	delete pS;
}
