#include <cstdlib>
#include <iostream>
#include <limits>
#include <string>
#include <vector>

#include "execute_functions.hpp"


std::string to_string(Template templ)
{
	std::string out;
	for (const auto& t : templ) {
		switch (t.type) {
		case TItem::Type::Base:
			out += t.base;
			break;

		case TItem::Type::Protection:
			out += '\\';
			out += std::to_string(t.prot.first);
			out += '\\';
			out += std::to_string(t.prot.second);
			break;

		case TItem::Type::Reference:
			out += '|' + std::to_string(t.ref) + '|';
			break;
		}
	}

	return out;
}


int main(int, char**)
{
	std::vector<std::pair<std::string, std::string>> tests;
	tests.push_back(std::make_pair("CIIC", "I"));
	tests.push_back(std::make_pair("FIIF", "C"));
	tests.push_back(std::make_pair("PIIC", "F"));
	tests.push_back(std::make_pair("ICIIF", "P"));
	tests.push_back(std::make_pair("IFICPCCPIIC", "\\2\\3"));
	tests.push_back(std::make_pair("IIPCICPIIF", "|5|"));

	std::cout << "1.." << tests.size() << '\n';

	Template output;
	std::string actual;

	for (size_t t = 0; t < tests.size(); ++t) {
		dna.clear();
		dna.insert(std::end(dna), std::begin(tests[t].first), std::end(tests[t].first));

		output = templates();

		actual = to_string(output);

		std::cout
			<< (tests[t].second != actual ? "not " : "") << "ok " << t + 1
			<< " - " << tests[t].first
			<< " -> " << actual
			<< " (" << tests[t].second << ")\n";
	}

	return EXIT_SUCCESS;
}
