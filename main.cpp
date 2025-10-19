// Compiler: MSVC 19.34 (Visual Studio 2022 v17.4)
// Standard : C++20

#include <iostream>
#include "innerProductTester.h"

int userInput()
{
	int number = 0;
	std::cin >> number;

	if (std::cin.fail() || std::cin.peek() != '\n')
	{
		std::cin.clear();
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		throw std::invalid_argument("\nInvalid input: please enter an integer.\n");
	}

	std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	return number;
}

int main()
{
	int amount = 0;
	int minNumber = 0;
	int maxNumber = 0;
	int mode = 0;
	int testNumber = 0;
	int seed = 0;

	std::cout << "Enter number of tests: ";
	testNumber = userInput();
	if (testNumber < 1)
	{
		std::cerr << "Number of tests must be at least 1!";
		return 0;
	}
	std::cout << "Enter seed for random generator(0 - random): ";
	seed = userInput();
	if (seed < 0)
		seed = abs(seed);

	for (int i = 0; i < testNumber; i++)
	{
		if (testNumber > 1)
			std::cout << "Test #" << i + 1 << std::endl;

		do
		{
			try
			{
				std::cout << "Enter the amount of random numbers in both vectors: ";
				amount = userInput();
				if (amount < 1)
					throw std::out_of_range("\nAmount of random numbers can't be less than one!\n");

				std::cout << "Enter the minimum possible number: ";
				minNumber = userInput();
				std::cout << "Enter the maximum possible number: ";
				maxNumber = userInput();

				if (minNumber == maxNumber)
					throw std::invalid_argument("\nMin number can't be equal to max number!\n");
				if (minNumber > maxNumber)
					std::swap(minNumber, maxNumber);

				std::cout << "Select computation mode:\n";
				std::cout << "0 - BASIC (a * b)\n";
				std::cout << "1 - HEAVY (sin(a) * cos(b))\n";
				std::cout << "2 - MODERATE (a*b if even, else 0)\n";
				std::cout << "Enter mode: ";

				mode = userInput();
				if (mode < 0 || mode > 2)
					throw std::invalid_argument("\nMode must be 0, 1 or 2!\n");
			}
			catch (std::out_of_range& e)
			{
				std::cout << e.what() << '\n';
				amount = 0;
			}
			catch (std::invalid_argument& i)
			{
				std::cout << i.what() << '\n';
				amount = 0;
			}
		} while (amount < 1);

		InnerProductTester tester(seed, amount, minNumber, maxNumber);
		tester.runAllTests(mode);

		std::cout << "\n";
	}
	return 0;
}