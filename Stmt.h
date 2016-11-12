#ifndef __STMT_H__
#define __STMT_H__

#include <vector>
#include <string>
#include <iostream>

class Statement
{
public:
	enum StatementType
	{
		FUNC = 0,
		IMPLICATION = 1,
		OR = 2,
		AND = 3,
		NOT = 4,
		OR_MUL = 5,
		AND_MUL = 6,
		StatementTypeNum,
	};

	StatementType sType;

	Statement * first;
	Statement * second;

	//std::vector<Statement *> childs;

	Statement(StatementType s) : sType(s), first(nullptr), second(nullptr) {};
	Statement(StatementType s, Statement * f, Statement * sec = nullptr) : sType(s), first(f), second(sec) {};
	virtual ~Statement() {};

	virtual void Print(std::ostream &, int) const; //For all except Func
	virtual Statement * DeepCopy() = 0;
	virtual Statement * ReduceImp(); //For all except Imp
	virtual Statement * DistributeNot(bool); //For And/Or
	virtual Statement * DistributeOrOnePass(bool &);
};

class FuncStmt : public Statement
{
public:
	std::string funcName;
	std::vector<std::string> args;

	FuncStmt() : Statement(StatementType::FUNC) {};
	FuncStmt(std::string name, std::vector<std::string> val) : Statement(StatementType::FUNC), funcName(name), args(val) {};

	void Print(std::ostream &, int) const;
	Statement * DeepCopy();
	Statement * DistributeNot(bool);
	Statement * DistributeOrOnePass(bool &) { return this; };
};

class ImpStmt : public Statement
{
public:
	ImpStmt() : Statement(StatementType::IMPLICATION) {};
	ImpStmt(Statement * l, Statement * r = nullptr) : Statement(StatementType::IMPLICATION, l, r) {};

	Statement * DeepCopy();
	Statement * ReduceImp();
	Statement * DistributeNot(bool) { std::cerr << "ReduceImp before DistributeNot!" << std::endl; return this; };
	Statement * DistributeOrOnePass(bool &) { std::cerr << "ReduceImp before DistributeOrOnePass!" << std::endl; return this; };
};

class OrStmt : public Statement
{
public:
	OrStmt() : Statement(StatementType::OR) {};
	OrStmt(Statement * l, Statement * r = nullptr) : Statement(Statement::OR, l, r) {};

	Statement * DeepCopy();
	virtual Statement * DistributeOrOnePass(bool &);
};

class AndStmt : public Statement
{
public:
	AndStmt() : Statement(StatementType::AND) {};
	AndStmt(Statement * l, Statement * r = nullptr) : Statement(StatementType::AND, l, r) {};

	Statement * DeepCopy();
	virtual Statement * DistributeOrOnePass(bool &);
};

class NotStmt : public Statement
{
public:
	NotStmt() : Statement(StatementType::NOT) {};
	NotStmt(Statement * n) : Statement(StatementType::NOT, n) {};

	Statement * DeepCopy();
	Statement * DistributeNot(bool isN);
	Statement * DistributeOrOnePass(bool &) { return this; };
};
/*
class OrMulStmt : public Statement
{
public:
	OrMulStmt() : Statement(StatementType::OR_MUL) {};
	OrMulStmt(const std::vector<Statement *> & val) : Statement(StatementType::OR_MUL, val) {};

};

class AndMulStmt : public Statement
{
public:
	AndMulStmt() : Statement(StatementType::AND_MUL) {};
	AndMulStmt(const std::vector<Statement *> & val) : Statement(StatementType::AND_MUL, val) {};

};
*/
void ReleaseAll(Statement *);
#endif // !__STMT_H__
