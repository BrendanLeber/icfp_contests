#include <cstdlib>
#include <iostream>
#include <limits>
#include <string>
#include <utility>
#include <vector>

#include "execute_functions.hpp"


int main(int, char**)
{
	std::vector<std::pair<std::string, std::string>> tests;
	tests.push_back(std::make_pair("C", "I"));
	tests.push_back(std::make_pair("F", "C"));
	tests.push_back(std::make_pair("P", "F"));
	tests.push_back(std::make_pair("IC", "P"));
	tests.push_back(std::make_pair("ICFP", "PCF"));

	std::cout << "1.." << tests.size() << '\n';

	DNA input, output;
	std::string actual;

	for (size_t t = 0; t < tests.size(); ++t) {
		dna.clear();
		dna.insert(std::end(dna), std::begin(tests[t].first), std::end(tests[t].first));

		output = consts();

		actual.clear();
		actual.insert(std::end(actual), std::begin(output), std::end(output));

		std::cout
			<< (tests[t].second != actual ? "not " : "") << "ok " << t + 1
			<< " - " << tests[t].first
			<< " -> " << actual
			<< " (" << tests[t].second << ")\n";
	}

	return EXIT_SUCCESS;
}
