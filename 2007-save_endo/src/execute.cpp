#include <algorithm>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <locale>
#include <sstream>
#include <string>

#include "execute.hpp"

int main(int argc, char** argv)
{
    if (argc < 2) {
        std::cerr << "Syntax: " << argv[0] << " <dna-file>...\n"; /* NOLINT */
        exit(EXIT_FAILURE);
    }

#if !defined(TRACE)
    std::cerr.imbue(std::locale(""));
    std::cout.imbue(std::locale(""));
#endif

    Arrow arrow;

    try {
        for (int arg = 1; arg < argc; ++arg) {
            arrow.read(argv[arg]); /* NOLINT */
        }

#if !defined(TRACE)
        std::cerr << "iteration " << std::fixed << arrow.iteration
                  << "  dna " << std::fixed << arrow.dna.length()
                  << "  rna " << std::fixed << arrow.rna
                  << "  cost " << std::fixed << arrow.cost << '\n';
#endif

        while (true) {
            arrow.execute();
        }
    }
    catch (std::ios_base::failure const& ex) {
        std::cerr << "exception " << ex.what() << " code " << ex.code() << '\n';
    }
    catch (std::exception const& ex) {
        std::cerr
            << "exception " << ex.what()
            << "\niterations " << arrow.iteration
            << "\ndna = " << arrow.to_string(arrow.dna) << " (" << std::fixed << arrow.dna.length() << " bases)"
            << "\ncost = " << std::fixed << arrow.cost
            << "\nlen(rna) = " << std::fixed << arrow.rna << "\n\n";
    }

    // we exit when matchreplace() calls finish().
    // getting here is an error
    return EXIT_FAILURE;
}
