#include <fstream>
#include <iomanip>
#include <iostream>
#include <locale>
#include <png++/png.hpp>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>
#include <zlib.h>

using Pixel = int;
using Bitmap = std::vector<Pixel>;

std::string read_prefix(std::string const& file_name);

std::string read_prefix(std::string const& file_name)
{
#if defined(TRACE)
    std::cerr << "read_prefix(" << file_name << ")\n";
#endif

    std::string prefix{};

    if (file_name != "null") {
	auto inf = std::ifstream(file_name);
	if (!inf.is_open()) {
	    std::stringstream ss;
	    ss << "Unable to open '" << file_name << "' for reading";
	    throw std::runtime_error(ss.str());
	}

	inf >> prefix;
	inf.close();
    }

    return prefix;
}

int main(int argc, char** argv)
{
    std::vector<std::string> args(argv + 1, argv + argc); // NOLINT
    if (args.size() != 3) {
	std::cerr << "Syntax: " << argv[0] << " <prefix-dna> <source.png> <target.png>\n"; // NOLINT
	return -1;
    }

    int ret_val = 0;
    try {
	auto prefix = read_prefix(args[0]);
	png::image<png::rgb_pixel> source(args[1], png::require_color_space<png::rgb_pixel>());
	png::image<png::rgb_pixel> target(args[2], png::require_color_space<png::rgb_pixel>());

	size_t incorrect_pixels = 0;

	if (source.get_width() != target.get_width() || source.get_height() != target.get_height()) {
	    std::cerr << "Image sizes do not match!\n";
	    ret_val = 1;
	}
	else {
	    for (size_t row = 0; row < source.get_height(); ++row) {
		for (size_t col = 0; col < source.get_width(); ++col) {
		    auto spixel = source.get_pixel(col, row);
		    auto tpixel = target.get_pixel(col, row);
		    if (spixel.red != tpixel.red || spixel.green != tpixel.green || spixel.blue != tpixel.blue) {
			++incorrect_pixels;
		    }
		}
	    }
	}

	auto risk = 10 * incorrect_pixels + prefix.size();

	std::cout.imbue(std::locale(""));
	std::cout << "risk " << std::fixed << risk << " (10 * " << incorrect_pixels << " + " << prefix.size() << ")\n";
    }
    catch (std::exception const& ex) {
	std::cerr << "Exception: " << ex.what() << ".\n";
	ret_val = 1;

    }

    return ret_val;
}
