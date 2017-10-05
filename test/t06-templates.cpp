#include <cstdlib>
#include <iostream>
#include <limits>
#include <string>
#include <vector>

#include "dna.hpp"

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

    Template output;
    std::string actual;

    for (size_t t = 0; t < tests.size(); ++t) {
        dna.clear();
        dna.insert(std::end(dna), std::begin(tests[t].first), std::end(tests[t].first));

        output = templates();
        actual = to_string(output);

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
