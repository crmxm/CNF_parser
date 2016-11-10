#include<iostream>

#include"Stmt.h"

int main()
{
	char buff[1024];

	int size = 1024;

	std::cin.getline(buff, size);

	int offset = 0;

	Statement * res = Parser(buff, offset);

	res->Print(std::cout, 0);

	return 0;
}