#include <iostream>
#include <cstdlib>

#include "utils.cpp"

int main(int argc, char** argv)
{
	if (argc != 2) {
		std::cerr << "Syntax: " << argv[0] << " <input.um>\n";
		return EXIT_FAILURE;
	}

	auto scroll = load(argv[1]);

	return 0;
}
