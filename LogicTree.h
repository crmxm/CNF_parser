#ifndef __LOGIC_TREE__
#define __LOGIC_TREE__

#include "Stmt.h"

class LogicTree
{
private:
	Statement * stmt;

	Statement * Parser(const char *, int &);
	//Statement * ReduceImp(Statement *);
	//Statement * DistributeNot(Statement *, bool);
	//Statement * DistributeOrOnePass(Statement *, bool &);

public:
	LogicTree() : stmt(nullptr) {};
	LogicTree(const char * in) { int index = 0; stmt = Parser(in, index); };
	LogicTree(const LogicTree &) = delete;
	LogicTree & operator = (const LogicTree &) = delete;
	~LogicTree() { ReleaseAll(stmt); };

	void Print(std::ostream & os) { stmt->Print(os, 0); };
	//void ReduceImp() { stmt = ReduceImp(stmt); };
	void ReduceImp() { Statement * temp = stmt; if (temp != (stmt = stmt->ReduceImp())) delete temp; };
	//void DistributeNot() { stmt = DistributeNot(stmt, false); };
	void DistributeNot() { Statement * temp = stmt; if (temp != (stmt = stmt->DistributeNot(false))) delete temp; };
	void DistributeOr();
};

#endif