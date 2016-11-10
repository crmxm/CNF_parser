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

	Statement(StatementType s) : sType(s) {};

	virtual void Print(std::ostream &, int) = 0;
};

class FuncStmt : public Statement
{
public:
	std::string funcName;
	std::vector<std::string> args;

	FuncStmt() : Statement(StatementType::FUNC) {};
	FuncStmt(std::string name, std::vector<std::string> val) : Statement(StatementType::FUNC), funcName(name), args(val) {};
	
	void Print(std::ostream &, int);
};

class ImpStmt : public Statement
{
public:
	Statement * left;
	Statement * right;

	ImpStmt() : Statement(StatementType::IMPLICATION), left(nullptr), right(nullptr) {};
	ImpStmt(Statement * l, Statement * r = nullptr) : Statement(StatementType::IMPLICATION), left(l), right(r) {};

	void Print(std::ostream &, int);
};

class OrStmt : public Statement
{
public:
	Statement * left;
	Statement * right;

	OrStmt() : Statement(StatementType::OR), left(nullptr), right(nullptr) {};
	OrStmt(Statement * l, Statement * r = nullptr) : Statement(StatementType::OR), left(l), right(r) {};

	void Print(std::ostream &, int);
};

class AndStmt : public Statement
{
public:
	Statement * left;
	Statement * right;

	AndStmt() : Statement(StatementType::OR), left(nullptr), right(nullptr) {};
	AndStmt(Statement * l, Statement * r = nullptr) : Statement(StatementType::AND), left(l), right(r) {};

	void Print(std::ostream &, int);
};

class NotStmt : public Statement
{
public:
	Statement * stmt;

	NotStmt() : Statement(StatementType::NOT), stmt(nullptr) {};
	NotStmt(Statement * n) : Statement(StatementType::NOT), stmt(n) {};

	void Print(std::ostream &, int);
};

class OrMulStmt : public Statement
{
public:
	std::vector<Statement *> stmts;

	OrMulStmt() : Statement(StatementType::OR_MUL) {};
	OrMulStmt(std::vector<Statement *> val) : Statement(StatementType::OR_MUL), stmts(val) {};

	void Print(std::ostream &, int);
};

class AndMulStmt : public Statement
{
public:
	std::vector<Statement *> stmts;

	AndMulStmt() : Statement(StatementType::AND_MUL) {};
	AndMulStmt(std::vector<Statement *> val) : Statement(StatementType::AND_MUL), stmts(val) {};

	void Print(std::ostream &, int);
};

class CNF
{
private:
	Statement * stmt;

public:
	CNF() {};
};

FuncStmt * ReadFunc(const char *, int &);
Statement * Parser(const char *, int &);
Statement * Compress(Statement *);
Statement * RmImp(Statement *);
Statement * ReduceNot(Statement *);

#endif // !__STMT_H__
