#include <cstdlib>
#include <fstream>
#include <iostream>
#include <stdexcept>

#include "rna.hpp"

void read(std::string const& rna_file)
{
    std::ifstream in(rna_file);
    std::string line;
    while (std::getline(in, line)) {
        rna.push_back(line);
    }
}

int main(int argc, char** argv)
{
    if (argc != 2) {
        std::cerr << "Syntax: " << argv[0] << " <rna-file>\n"; /* NOLINT */
        exit(EXIT_FAILURE);
    }

    rna_init();
    read(argv[1]); /* NOLINT */

    try {
        build();
    }
    catch (std::exception const& ex) {
        std::cerr << "exception!!! " << ex.what() << '\n';
    }

    return EXIT_SUCCESS;
}
