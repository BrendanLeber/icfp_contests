#include <cstdlib>
#include <iostream>
#include <limits>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

#include "dna.hpp"

int main()
{
    int retval = EXIT_SUCCESS;

    // protection level, input dna, expected output
    using Test = std::tuple<Number, std::string, std::string>;
    std::vector<Test> tests;
    tests.emplace_back(std::make_tuple(0, "I", "I"));
    tests.emplace_back(std::make_tuple(1, "I", "C"));
    tests.emplace_back(std::make_tuple(2, "I", "F"));
    tests.emplace_back(std::make_tuple(3, "I", "P"));
    tests.emplace_back(std::make_tuple(4, "I", "IC"));
    tests.emplace_back(std::make_tuple(5, "I", "CF"));

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

        if (std::get<2>(tval) != actual) {
            retval = EXIT_FAILURE;
        }

        std::cout
            << (std::get<2>(tval) != actual ? "not " : "") << "ok " << t + 1
            << " - protect(" << std::get<0>(tval) << ", '" << std::get<1>(tval) << "')"
            << " -> " << actual
            << " [ " << std::get<2>(tval) << " ]\n";
    }

    return retval;
}
