#include <iostream>
#include <iomanip>
#include <string>
#include <cstdlib>
#include <ctime>
#include "BTree.h"

int main()
{
		std::srand(std::time(0));
		BTree<int, 3> myTree;
		try
		{
			int gens[256] = { 0 };
			for (unsigned short int i = 0; i < 256u; ++i)
			{
				int tmp = std::rand();
				std::cout << "GENERATED " << tmp << std::endl;
				gens[i] = tmp;
				myTree.insert(tmp);
			}
			for (unsigned short int i = 0; i < 256u; ++i)
				std::cout << std::setw(5) << std::right << gens[i] << " is " << (myTree.isExist(gens[i]) ? "exists" : "not exists") << std::endl;
			int tmp;
			std::cout << "element for search: ";
			std::cin >> tmp;
			if (myTree.isExist(tmp))
				std::cout << "element is exists" << std::endl;
			else
				std::cout << "element is not exists" << std::endl;
		}
		catch (std::exception err)
		{
			std::cout << err.what() << std::endl;
		}
		return 0;
}