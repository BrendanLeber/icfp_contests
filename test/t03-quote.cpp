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
    tests.emplace_back(std::make_pair("I", "C"));
    tests.emplace_back(std::make_pair("C", "F"));
    tests.emplace_back(std::make_pair("F", "P"));
    tests.emplace_back(std::make_pair("P", "IC"));
    tests.emplace_back(std::make_pair("ICFP", "CFPIC"));

    std::cout << "1.." << tests.size() << '\n';

    DNA input, output;
    std::string actual;

    for (size_t t = 0; t < tests.size(); ++t) {
        input.clear();
        input.insert(std::end(input), std::begin(tests[t].first), std::end(tests[t].first));

        output = quote(input);

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
