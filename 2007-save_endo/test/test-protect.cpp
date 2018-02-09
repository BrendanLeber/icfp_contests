#include <cstdlib>
#include <iostream>
#include <limits>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

#include "execute.hpp"

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

    Arrow arrow;
    size_t test = 0;

    for (auto& tval : tests) {
        test++;

        arrow.dna = std::get<1>(tval);
        auto output = arrow.protect(std::get<0>(tval), arrow.dna);
        auto actual = output.to_string();

        if (std::get<2>(tval) != actual) {
            retval = EXIT_FAILURE;
        }

        std::cout << (std::get<2>(tval) != actual ? "not " : "") << "ok " << test
                  << " - protect(" << std::get<0>(tval) << ", '" << std::get<1>(tval) << "')"
                  << " expected " << std::get<2>(tval) << " actual " << actual << '\n';
    }

    return retval;
}
