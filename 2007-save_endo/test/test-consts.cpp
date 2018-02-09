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

    std::vector<std::pair<std::string, std::string>> tests;
    tests.emplace_back(std::make_pair("C", "I"));
    tests.emplace_back(std::make_pair("F", "C"));
    tests.emplace_back(std::make_pair("P", "F"));
    tests.emplace_back(std::make_pair("IC", "P"));
    tests.emplace_back(std::make_pair("ICFP", "PCF"));

    std::cout << "1.." << tests.size() << '\n';

    Arrow arrow;

    for (size_t t = 0; t < tests.size(); ++t) {
        auto expected = tests[t].second;

        arrow.dna = tests[t].first;
        auto actual = arrow.consts().to_string();

        if (expected != actual) {
            retval = EXIT_FAILURE;
        }

        std::cout
            << (expected != actual ? "not " : "") << "ok " << t + 1 << " - "
            << tests[t].first << " expected " << expected << " actual " << actual << '\n';
    }

    return retval;
}
