#include <cstdlib>
#include <iostream>
#include <limits>

#include "execute_functions.hpp"


int main(int, char**)
{
	std::cout << "1..2\n";

	{
		std::string test_dna { "CIIC" };
		std::string expected { "I" };

		dna.clear();
		dna.insert(std::end(dna), std::begin(test_dna), std::end(test_dna));

		auto actual = pattern();

		std::cout
			<< (expected != actual ? "not " : "") << "ok 1"
			<< " - " << test_dna
			<< " -> " << actual
			<< " (" << expected << ")\n";
	}

	{
		std::string test_dna { "IIPIPICPIICICIIF" };
		std::string expected { "(!2)P" };

		dna.clear();
		dna.insert(std::end(dna), std::begin(test_dna), std::end(test_dna));

		auto actual = pattern();

		std::cout
			<< (expected != actual ? "not " : "") << "ok 2"
			<< " - " << test_dna
			<< " -> " << actual
			<< " (" << expected << ")\n";
	}

	return EXIT_SUCCESS;
}
