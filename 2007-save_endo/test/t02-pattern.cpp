#include <cstdlib>
#include <iostream>
#include <string>

#include "dna.hpp"

int main()
{
    int retval = EXIT_SUCCESS;
    std::cout << "1..2\n";

    {
        std::string test_dna{"CIIC"};
        std::string expected{"I"};

        dna.clear();
        dna.insert(std::end(dna), std::begin(test_dna), std::end(test_dna));

        auto actual = to_string(pattern());
        if (expected != actual) {
            retval = EXIT_FAILURE;
        }

        std::cout
            << (expected != actual ? "not " : "") << "ok 1"
            << " - " << test_dna
            << " -> " << actual
            << " (" << expected << ")\n";
    }

    {
        std::string test_dna{"IIPIPICPIICICIIF"};
        std::string expected{"(!2)P"};

        dna.clear();
        dna.insert(std::end(dna), std::begin(test_dna), std::end(test_dna));

        auto actual = to_string(pattern());
        if (expected != actual) {
            retval = EXIT_FAILURE;
        }

        std::cout
            << (expected != actual ? "not " : "") << "ok 2"
            << " - " << test_dna
            << " -> " << actual
            << " (" << expected << ")\n";
    }

    return retval;
}
