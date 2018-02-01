#include <cstdlib>
#include <fstream>
#include <iostream>
#include <stdexcept>

#if defined(COUNT_UNHANDLED_RNA)
#include <map>
#endif

#include "build.hpp"

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

#if defined(COUNT_UNHANDLED_RNA)
    std::map<std::string, unsigned> unhandled_rna;
#endif

    try {
	    static size_t counter = 0;

	    for (const auto& r : rna) {
		    ++counter;

		    if (r == "PIPIIIC") {
			    addColor(black);
		    }
		    else if (r == "PIPIIIP") {
			    addColor(red);
		    }
		    else if (r == "PIPIICC") {
			    addColor(green);
		    }
		    else if (r == "PIPIICF") {
			    addColor(yellow);
		    }
		    else if (r == "PIPIICP") {
			    addColor(blue);
		    }
		    else if (r == "PIPIIFC") {
			    addColor(magenta);
		    }
		    else if (r == "PIPIIFF") {
			    addColor(cyan);
		    }
		    else if (r == "PIPIIPC") {
			    addColor(white);
		    }
		    else if (r == "PIPIIPF") {
			    addColor(transparent);
		    }
		    else if (r == "PIPIIPP") {
			    addColor(opaque);
		    }
		    else if (r == "PIIPICP") {
			    bucket.clear();
		    }
		    else if (r == "PIIIIIP") {
			    position = move(position, dir);
		    }
		    else if (r == "PCCCCCP") {
			    dir = turnCounterClockwise(dir);
		    }
		    else if (r == "PFFFFFP") {
			    dir = turnClockwise(dir);
		    }
		    else if (r == "PCCIFFP") {
			    mark = position;
		    }
		    else if (r == "PFFICCP") {
			    line(position, mark);
		    }
		    else if (r == "PIIPIIP") {
			    tryfill();
		    }
		    else if (r == "PCCPFFP") {
			    addBitmap();
		    }
		    else if (r == "PFFPCCP") {
			    compose();
		    }
		    else if (r == "PFFICCF") {
			    clip();
		    }
#if defined(COUNT_UNHANDLED_RNA)
		    else {
			    auto it = unhandled_rna.find(r);
			    if (it == std::end(unhandled_rna)) {
				    unhandled_rna.emplace(std::make_pair(r, 1));
			    }
			    else {
				    it->second++;
			    }
		    }
#endif

#if !defined(TRACE)
		    if ((counter % 8192) == 0) {
#endif
			    std::cerr << counter << " of " << rna.size() << " (" << (counter * 100 / rna.size()) << "%)\n";
#if !defined(TRACE)
		    }
#endif
	    }

	    draw(bitmaps[0]);

    }
    catch (std::exception const& ex) {
	    std::cerr << "exception!!! " << ex.what() << '\n';
    }

#if defined(COUNT_UNHANDLED_RNA)
    unsigned total_unused = 0;
    std::cout << "Unhandled RNA Counts:\n";
    for (auto const& kv : unhandled_rna) {
	    total_unused += kv.second;
	    std::cout << "  " << kv.first << ' ' << kv.second << '\n';
    }
    std::cout << " total " << total_unused << '\n';
#endif

    return EXIT_SUCCESS;
}
