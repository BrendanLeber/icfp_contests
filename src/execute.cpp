#include <cstdlib>
#include <iostream>

#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "execute_functions.hpp"


void read(std::string dna_file)
{
	auto fd = open(dna_file.c_str(), O_RDONLY);
	if (fd == -1) {
		perror("open");
		exit(EXIT_FAILURE);
	}

	struct stat sb;
	if (fstat(fd, &sb) == -1) {
		perror("fstat");
		exit(EXIT_FAILURE);
	}

	auto pmap = reinterpret_cast<Base*>(mmap(NULL, sb.st_size, PROT_READ, MAP_SHARED, fd, 0));
	if (pmap == MAP_FAILED) {
		perror("mmap");
		exit(EXIT_FAILURE);
	}

	dna.insert(std::end(dna), pmap, pmap + sb.st_size);

	munmap(pmap, sb.st_size);
	close(fd);
}


int main(int argc, char** argv)
{
	if (argc != 2) {
		std::cerr << "Syntax: " << argv[0] << " <dna-file>\n";
		exit(EXIT_FAILURE);
	}

	read(argv[1]);
	execute();

	// we exit when matchreplace() calls finish().
	// getting here is an error
	return EXIT_FAILURE;
}
