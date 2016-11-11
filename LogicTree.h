#ifndef __LOGIC_TREE__
#define __LOGIC_TREE__

#include "Stmt.h"

class LogicTree
{
private:
	Statement * stmt;

	Statement * Parser(const char *, int &);
	void ReleaseAll(Statement *);
	Statement * ReduceImp(Statement *);
	Statement * DistributeNot(Statement *, bool);
	//int EvaluateDisorderCount(Statement *, int);
	Statement * DistributeOrOnePass(Statement *, bool &);
	//Statement * CompressTree(Statement *);

public:
	LogicTree() : stmt(nullptr) {};
	LogicTree(const char * in) { int index = 0; stmt = Parser(in, index); };
	LogicTree(const LogicTree &) = delete;
	LogicTree & operator = (const LogicTree &) = delete;
	~LogicTree() { ReleaseAll(stmt); };

	void Print(std::ostream & os) { stmt->Print(os, 0); };
	void ReduceImp() { stmt = ReduceImp(stmt); };
	void DistributeNot() { stmt = DistributeNot(stmt, false); };
	//int EvaluateDisorderCount() { return EvaluateDisorderCount(stmt, 0); };
	void DistributeOr();
	//void CompressTree() { stmt = CompressTree(stmt); };
};

#endif