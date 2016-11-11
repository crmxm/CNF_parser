#include <iostream>

#include "Stmt.h"

const char StatementTypeName[5][4] = { "", "Imp", "Or", "And", "Not" };

void Statement::Print(std::ostream & os, int depth)
{
	os << std::string(depth, '|') << StatementTypeName[sType] << std::endl;

	first->Print(os, depth + 1);

	if (second)
		second->Print(os, depth + 1);
}

void FuncStmt::Print(std::ostream & os, int depth)
{
	os << std::string(depth, '|') << funcName << '(';

	for (auto name : args)
		os << name << ',';

	os << ')' << std::endl;
}

Statement * FuncStmt::DeepCopy()
{
	return new FuncStmt(funcName, args);
}

Statement * ImpStmt::DeepCopy()
{
	return new ImpStmt(first->DeepCopy(), second->DeepCopy());
}

Statement * OrStmt::DeepCopy()
{
	return new OrStmt(first->DeepCopy(), second->DeepCopy());
}

Statement * AndStmt::DeepCopy()
{
	return new AndStmt(first->DeepCopy(), second->DeepCopy());
}

Statement * NotStmt::DeepCopy()
{
	return new NotStmt(first->DeepCopy());
}