#include<iostream>

#include"LogicTree.h"

int main()
{
	char buff[1024];

	int size = 1024;

	std::cin.getline(buff, size);

	int offset = 0;

	LogicTree * pLT = new LogicTree(buff);
	pLT->Print(std::cout);

	std::cout << "Reduce Imp...Done." << std::endl;

	pLT->ReduceImp();
	pLT->Print(std::cout);

	std::cout << "Distribute Not...Done." << std::endl;

	pLT->DistributeNot();
	pLT->Print(std::cout);

	std::cout << "Distribute Or...Done." << std::endl;

	pLT->DistributeOr();
	pLT->Print(std::cout);

	//int x = pLT->EvaluateDisorderCount();

	//std::cout << "Compress Tree...Done." << std::endl;

	//pLT->CompressTree();
	//pLT->Print(std::cout);

	delete pLT;

	return 0;
}