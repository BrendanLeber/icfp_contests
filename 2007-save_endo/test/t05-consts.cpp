#include <cstdlib>
#include <iostream>
#include <limits>
#include <string>
#include <utility>
#include <vector>

#include "dna.hpp"

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

    DNA input, output;
    std::string actual;

    for (size_t t = 0; t < tests.size(); ++t) {
        dna.clear();
        dna.insert(std::end(dna), std::begin(tests[t].first), std::end(tests[t].first));

        output = consts();

        actual.clear();
        actual.insert(std::end(actual), std::begin(output), std::end(output));

        if (tests[t].second != actual) {
            retval = EXIT_FAILURE;
        }

        std::cout
            << (tests[t].second != actual ? "not " : "") << "ok " << t + 1
            << " - " << tests[t].first
            << " -> " << actual
            << " (" << tests[t].second << ")\n";
    }

    return retval;
}
