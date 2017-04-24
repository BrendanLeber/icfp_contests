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
	tests.push_back(std::make_pair("I", "C"));
	tests.push_back(std::make_pair("C", "F"));
	tests.push_back(std::make_pair("F", "P"));
	tests.push_back(std::make_pair("P", "IC"));
	tests.push_back(std::make_pair("ICFP", "CFPIC"));

	std::cout << "1.." << tests.size() << '\n';

	DNA input, output;
	std::string actual;

	for (size_t t = 0; t < tests.size(); ++t) {
		input.clear();
		input.insert(std::end(input), std::begin(tests[t].first), std::end(tests[t].first));

		output = quote(input);

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
