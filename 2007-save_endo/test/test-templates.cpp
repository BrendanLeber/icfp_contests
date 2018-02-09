#include <cstdlib>
#include <iostream>
#include <limits>
#include <string>
#include <vector>

#include "execute.hpp"

int main()
{
    int retval = EXIT_SUCCESS;

    std::vector<std::pair<std::string, std::string>> tests;
    tests.emplace_back(std::make_pair("CIIC", "I"));
    tests.emplace_back(std::make_pair("FIIF", "C"));
    tests.emplace_back(std::make_pair("PIIC", "F"));
    tests.emplace_back(std::make_pair("ICIIF", "P"));
    tests.emplace_back(std::make_pair("IFICPCCPIIC", R"(\3)"));
    tests.emplace_back(std::make_pair("IIPCICPIIF", "|5|"));

    std::cout << "1.." << tests.size() << '\n';

    Arrow arrow;

    Template output;
    std::string actual;

    for (size_t t = 0; t < tests.size(); ++t) {
        auto const& test_case = tests[t].first;
        arrow.dna = test_case;

        auto const& expected = tests[t].second;
        auto actual = Arrow::to_string(arrow.templates());

        if (expected != actual) {
            retval = EXIT_FAILURE;
        }

        std::cout
            << (expected != actual ? "not " : "") << "ok " << t + 1 << " - "
            << test_case << " expected " << expected << " actual " << actual << '\n';
    }

    return retval;
}
