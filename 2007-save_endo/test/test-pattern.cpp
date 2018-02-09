#include <cstdlib>
#include <iostream>
#include <string>

#include "execute.hpp"

int main()
{
    int retval = EXIT_SUCCESS;

    Arrow arrow{};

    std::cout << "1..2\n";

    {
        std::string test_dna{ "CIIC" };
        std::string expected{ "I" };

        arrow.dna = test_dna;
        auto actual = Arrow::to_string(arrow.pattern());
        if (expected != actual) {
            retval = EXIT_FAILURE;
        }

        std::cout
            << (expected != actual ? "not " : "") << "ok 1"
            << " - " << test_dna << " expected " << expected << " acutal " << actual << '\n';
    }

    {
        std::string test_dna{ "IIPIPICPIICICIIF" };
        std::string expected{ "(!2)P" };

        arrow.dna = test_dna;
        auto actual = Arrow::to_string(arrow.pattern());
        if (expected != actual) {
            retval = EXIT_FAILURE;
        }

        std::cout
            << (expected != actual ? "not " : "") << "ok 2"
            << " - " << test_dna << " expected " << expected << " acutal " << actual << '\n';
    }

    return retval;
}
