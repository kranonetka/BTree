#include "BTree.h"
#include <iostream>
#include <iomanip>
#include <ctime>

int main()
{
	std::srand(std::time(0));
	BTree<int, 3> myTree;
	std::cout << "count of elements in tree: ";
	unsigned int n;
	std::cin >> n;
	int *gens = new int[n];
	for (unsigned int i = 0u; i < n; ++i)
	{
		int tmp = std::rand();
		gens[i] = tmp;
		//std::cout << "GENERATED " << tmp << std::endl;
		myTree.insert(tmp);
	}
	for (unsigned int i = 0u; i < n; ++i)
		std::cout << std::setw(5) << std::right << gens[i] << " is " << (myTree.isExist(gens[i]) ? "exists" : "not exists") << std::endl;
	for (unsigned int i = 0u; i < n; ++i)
	{
		int tmp = std::rand();
		std::cout << std::setw(5) << std::right << tmp << " is " << (myTree.isExist(tmp) ? "exists" : "not exists") << std::endl;
	}
	myTree.traverse();
	std::cout << std::endl;
	while (1)
	{
		int tmp;
		std::cout << "element for delete: ";
		std::cin >> tmp;
		try
		{
			myTree.remove(tmp);
			myTree.traverse();
			std::cout << std::endl;
			std::cout << std::setw(5) << std::right << tmp << " is " << (myTree.isExist(tmp) ? "exists" : "not exists") << std::endl;
			std::cout << std::endl;
		}
		catch (std::exception err)
		{
			std::cout << err.what() << std::endl;
			return 1;
		}
	}
	delete[] gens;
	return 0;
}