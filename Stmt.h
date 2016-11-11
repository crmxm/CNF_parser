#ifndef __STMT_H__
#define __STMT_H__

#include <vector>
#include <string>

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

	std::vector<Statement *> childs;

	Statement(StatementType s) : sType(s) {};
	Statement(StatementType s, int n) : sType(s) { childs.reserve(n); };
	Statement(StatementType s, const std::vector<Statement*> & val) : sType(s), childs(val) {};
	virtual ~Statement() {};

	virtual void Print(std::ostream &, int);

	void AddChild(Statement * pS) { childs.push_back(pS); };
};

class FuncStmt : public Statement
{
public:
	std::string funcName;
	std::vector<std::string> args;

	FuncStmt() : Statement(StatementType::FUNC, 0) {};
	FuncStmt(std::string name, std::vector<std::string> val) : Statement(StatementType::FUNC, 0), funcName(name), args(val) {};
	
	void Print(std::ostream &, int);
};

class ImpStmt : public Statement
{
public:
	ImpStmt() : Statement(StatementType::IMPLICATION, 2) {};
	ImpStmt(Statement * l) : Statement(StatementType::IMPLICATION, 2) { AddChild(l); };
	ImpStmt(Statement * l, Statement * r) : Statement(StatementType::IMPLICATION, 2) { AddChild(l); AddChild(r); };

};

class OrStmt : public Statement
{
public:
	OrStmt() : Statement(StatementType::OR, 2) {};
	OrStmt(Statement * l) : Statement(StatementType::OR, 2) { AddChild(l); };
	OrStmt(Statement * l, Statement * r) : Statement(Statement::OR, 2) { AddChild(l); AddChild(r); };

};

class AndStmt : public Statement
{
public:
	AndStmt() : Statement(StatementType::AND, 2) {};
	AndStmt(Statement * l) : Statement(StatementType::AND, 2) { AddChild(l); };
	AndStmt(Statement * l, Statement * r) : Statement(StatementType::AND, 2) { AddChild(l); AddChild(r); };

};

class NotStmt : public Statement
{
public:
	NotStmt() : Statement(StatementType::NOT, 1) {};
	NotStmt(Statement * n) : Statement(StatementType::NOT, 1) { AddChild(n); };

};

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

Statement * Compress(Statement *);
Statement * RmImp(Statement *);
Statement * ReduceNot(Statement *);

class LogicTree
{
private:
	Statement * stmt;

	Statement * Parser(const char *, int &);
	void ReleaseAll(Statement *);
	Statement * ReduceImp(Statement *);
	Statement * DistributeNot(Statement *, bool);
	Statement * CompressTree(Statement *);

public:
	LogicTree() : stmt(nullptr) {};
	LogicTree(const char * in) { int index = 0; stmt = Parser(in, index); };
	LogicTree(const LogicTree &) = delete;
	LogicTree & operator = (const LogicTree &) = delete;
	~LogicTree() { ReleaseAll(stmt); };

	void Print(std::ostream & os) { stmt->Print(os, 0); };
	void ReduceImp() { stmt = ReduceImp(stmt); };
	void DistributeNot() { stmt = DistributeNot(stmt, false); };
	void CompressTree() { stmt = CompressTree(stmt); };
};

#endif // !__STMT_H__
