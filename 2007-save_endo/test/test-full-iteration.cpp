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
    tests.emplace_back(std::make_pair("IIPIPICPIICICIIFICCIFPPIICCFPC", "PICFC"));
    tests.emplace_back(std::make_pair("IIPIPICPIICICIIFICCIFCCCPPIICCFPC", "PIICCFCFFPC"));
    tests.emplace_back(std::make_pair("IIPIPIICPIICIICCIICFCFC", "I"));

    std::cout << "1.." << tests.size() << '\n';

    Arrow arrow;

    Template output;
    std::string actual;

    for (size_t t = 0; t < tests.size(); ++t) {
        auto const& test_case = tests[t].first;
        arrow.dna = test_case;

        auto const& expected = tests[t].second;
        arrow.execute(); // Give me a ping, Vasili.  One ping only, please.
        auto actual = arrow.dna.to_string();

        if (expected != actual) {
            retval = EXIT_FAILURE;
        }

        std::cout
            << (expected != actual ? "not " : "") << "ok " << t + 1 << " - "
            << test_case << " expected " << expected << " actual " << actual << '\n';
    }

    return retval;
}
