#include <cstdlib>
#include <iostream>
#include <limits>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

#include "execute_functions.hpp"


int main(int, char**)
{
	// protection level, input dna, expected output
	using Test = std::tuple<Number, std::string, std::string>;
	std::vector<Test> tests;
	tests.push_back(std::make_tuple(0, "I", "I"));
	tests.push_back(std::make_tuple(1, "I", "C"));
	tests.push_back(std::make_tuple(2, "I", "F"));
	tests.push_back(std::make_tuple(3, "I", "P"));
	tests.push_back(std::make_tuple(4, "I", "IC"));
	tests.push_back(std::make_tuple(5, "I", "CF"));

	std::cout << "1.." << tests.size() << '\n';

	DNA input, output;
	std::string actual;

	for (size_t t = 0; t < tests.size(); ++t) {
		auto tval = tests[t];
		input.clear();
		input.insert(std::end(input), std::begin(std::get<1>(tval)), std::end(std::get<1>(tval)));

		output = protect(std::get<0>(tval), input);

		actual.clear();
		actual.insert(std::end(actual), std::begin(output), std::end(output));

		std::cout
			<< (std::get<2>(tval) != actual ? "not " : "") << "ok " << t + 1
			<< " - protect(" << std::get<0>(tval) << ", '" << std::get<1>(tval) << "')"
			<< " -> " << actual
			<< " [ " << std::get<2>(tval) << " ]\n";
	}

	return EXIT_SUCCESS;
}
