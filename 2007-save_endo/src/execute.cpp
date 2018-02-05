#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <fstream>
#include <iostream>

#include "dna.hpp"

void read(std::string const& dna_file)
{
    std::string data;
    std::ifstream inf;
    inf.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    inf.open(dna_file);
    inf >> data;
    inf.close();

    // remove any whitespace that might be in the input file
    data.erase(std::remove_if(std::begin(data), std::end(data),
                   [](char base) { return std::isspace(base); }),
        std::end(data));

    dna.insert(std::end(dna), std::begin(data), std::end(data));
}

int main(int argc, char** argv)
{
    if (argc < 2) {
        std::cerr << "Syntax: " << argv[0] << " <dna-file>...\n"; /* NOLINT */
        exit(EXIT_FAILURE);
    }

    for (int arg = 1; arg < argc; ++arg) {
        read(argv[arg]); /* NOLINT */
    }

    execute();

    // we exit when matchreplace() calls finish().
    // getting here is an error
    return EXIT_FAILURE;
}
