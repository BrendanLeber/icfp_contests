#include <cstdlib>
#include <iostream>
#include <limits>
#include <vector>

#include "execute.hpp"

int main()
{
    int retval = EXIT_SUCCESS;

    Arrow arrow;

    std::vector<Number> expected{
        0, std::numeric_limits<Number>::max(),
        2, 3, 4, 5, 6, 7,
        254, 255, 256, 257,
        65534, 65535, 65536, 65537,
        16777214, 16777215, 16777216, 16777277
    };

    std::cout << "1.." << expected.size() << '\n';
    for (size_t t = 0; t < expected.size(); ++t) {
        auto value = Arrow::asnat(expected[t]);
        arrow.dna = value;
        auto actual = arrow.nat();

        if (expected[t] != actual) {
            retval = EXIT_FAILURE;
        }

        std::cout
            << (expected[t] != actual ? "not " : "") << "ok " << (t + 1)
            << " - " << value << " -> " << actual << " (" << expected[t] << ")\n";
    }

    return retval;
}
