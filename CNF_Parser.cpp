#include<iostream>

#include"Stmt.h"

int main()
{
	std::cout << "All the leaves are brown" << std::endl;

	char buff[1024];

	int size = 1024;

	std::cin.getline(buff, size);

	int offset = 0;

	Statement * res = Parser(buff, offset);

	res->print(std::cout, 0);

	return 0;
}