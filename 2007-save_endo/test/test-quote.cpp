#include <cstdlib>
#include <iostream>
#include <limits>
#include <string>
#include <utility>
#include <vector>

#include "execute.hpp"

int main()
{
    int retval = EXIT_SUCCESS;

    Arrow arrow;

    std::vector<std::pair<std::string, std::string>> tests;
    tests.emplace_back(std::make_pair("I", "C"));
    tests.emplace_back(std::make_pair("C", "F"));
    tests.emplace_back(std::make_pair("F", "P"));
    tests.emplace_back(std::make_pair("P", "IC"));
    tests.emplace_back(std::make_pair("ICFP", "CFPIC"));

    std::cout << "1.." << tests.size() << '\n';

    std::string actual;
    for (size_t t = 0; t < tests.size(); ++t) {
        arrow.dna = tests[t].first;
        auto output = arrow.quote(arrow.dna);

        auto actual = output.to_string();
        if (tests[t].second != actual) {
            retval = EXIT_FAILURE;
        }

        std::cout
            << (tests[t].second != actual ? "not " : "") << "ok " << t + 1
            << " - " << tests[t].first << " expected " << tests[t].second << " actual " << actual << '\n';
    }

    return retval;
}
