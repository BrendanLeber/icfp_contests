#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <iostream>

#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "dna.hpp"

void read(std::string const& dna_file)
{
    auto fd = open(dna_file.c_str(), O_RDONLY); /* NOLINT */
    if (fd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    struct stat sb {
        0
    };
    if (fstat(fd, &sb) == -1) {
        perror("fstat");
        exit(EXIT_FAILURE);
    }

    auto pmap = static_cast<Base*>(mmap(nullptr, sb.st_size, PROT_READ, MAP_SHARED, fd, 0));
    if (pmap == MAP_FAILED) { /* NOLINT */
        perror("mmap");
        exit(EXIT_FAILURE);
    }

    dna.insert(std::end(dna), pmap, pmap + sb.st_size);

    munmap(pmap, sb.st_size);
    close(fd);

    // remove any whitespace that might be in the input file that was
    // added to our DNA sequence
    dna.erase(std::remove_if(std::begin(dna), std::end(dna),
                  [](DNA::value_type base) { return std::isspace(base); }),
        std::end(dna));
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
