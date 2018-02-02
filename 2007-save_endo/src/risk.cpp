#include <array>
#include <cerrno>
#include <cstring>
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

// Bitmap read_png_image(std::string const& file_name);
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

// Bitmap read_png_image(std::string const& file_name)
// {
// #if defined(TRACE)
// 	std::cerr << "read_png_image(" << file_name << ")\n";
// #endif

// 	auto fp = std::fopen(file_name.c_str(), "rb");
// 	if (fp == nullptr) {
// 		throw std::runtime_error(std::strerror(errno));
// 	}

// 	// check the PNG file signature
// 	std::array<unsigned char, 8> sig{};
// 	std::fread(sig.data(), 1, sig.size(), fp);
// 	if (png_check_sig(sig.data(), sig.size()) == 0) {
// 		std::stringstream ss;
// 		ss << "Invalid PNG signature in '" << file_name << "'";
// 		throw std::runtime_error(ss.str());
// 	}

// 	auto png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
// 	if (png_ptr == nullptr) {
// 		throw std::runtime_error("could not allocate PNG read struct");
// 	}

// 	auto info_ptr = png_create_info_struct(png_ptr);
// 	if (info_ptr == nullptr) {
// 		png_destroy_read_struct(&png_ptr, nullptr, nullptr);
// 		throw std::runtime_error("could not allocate PNG info struct");
// 	}

// 	if (setjmp(png_jmpbuf(png_ptr))) { // NOLINT
// 		png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
// 		throw std::runtime_error("error during PNG reading");
// 	}

// 	png_init_io(png_ptr, fp);
// 	png_set_sig_bytes(png_ptr, 8);
// 	png_read_info(png_ptr, info_ptr);

// 	auto width = png_get_image_width(png_ptr, info_ptr);
// 	auto height = png_get_image_height(png_ptr, info_ptr);
// 	auto color_type = png_get_color_type(png_ptr, info_ptr);
// 	auto bit_depth = png_get_bit_depth(png_ptr, info_ptr);

// 	if (color_type != PNG_COLOR_TYPE_RGB || bit_depth != 8) {
// 		png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
// 		std::stringstream ss;
// 		ss << "Invalid PNG format.  color_type " << color_type << " bit-depth " << bit_depth;
// 		throw std::runtime_error(ss.str());
// 	}

// 	auto row_pointers = static_cast<png_bytep*>(malloc(sizeof(png_bytep) * height)); // NOLINT
// 	auto row_bytes = png_get_rowbytes(png_ptr, info_ptr);
// 	for (int row = 0; row < height; ++row) {
// 		row_pointers[row] = static_cast<png_byte*>(malloc(row_bytes)); // NOLINT
// 	}

// 	png_read_image(png_ptr, row_pointers);

// 	auto bmp = Bitmap{};
// 	bmp.reserve(height * width);
// 	for (int row = 0; row < height; ++row) {
// 		auto rowp = row_pointers[row];
// 		for (int col = 0; col < width; ++col) {
// 			auto ppix = &(rowp[col * 4]);
// 			Pixel pixel = ((ppix[0] & 0xFF) << 16) | ((ppix[1] & 0xFF) <<  8) | (ppix[2] & 0xFF);
// 			bmp.push_back(pixel);
// 		}

// 		free(rowp);  // NOLINT
// 		row_pointers[row] = nullptr;
// 	}

// 	free(row_pointers);  // NOLINT
// 	png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);

// 	return bmp;
// }

int main(int argc, char** argv)
{
	std::vector<std::string> args(argv + 1, argv + argc); // NOLINT
	if (args.size() != 3) {
		std::cerr << "Syntax: " << argv[0] << " <prefix-dna> <source.png> <target.png>\n"; // NOLINT
		return -1;
	}

// #if defined(TRACE)
// 	std::cerr << "Compiled with libpng " << PNG_LIBPNG_VER_STRING
// 		  << "; using libpng " << png_libpng_ver << ".\n"; // NOLINT
// 	std::cerr << "Compiled with zlib " << ZLIB_VERSION
// 		  << "; using zlib " << zlib_version << ".\n";
// #endif

	int ret_val = 0;
	try {
		auto prefix = read_prefix(args[0]);
#if defined(TRACE)
		std::cerr << "prefix '" << prefix << "' (" << prefix.size() << ")\n";
#endif

		png::image<png::rgb_pixel> source(args[1], png::require_color_space<png::rgb_pixel>());
		png::image<png::rgb_pixel> target(args[2], png::require_color_space<png::rgb_pixel>());
		// auto source = read_png_image(args[1]);
		// auto target = read_png_image(args[2]);

		size_t incorrect_pixels = 0;

		if (source.get_width() != target.get_width() || source.get_height() != target.get_height()) {
			std::cerr << "Image sizes do not match!\n";
#if defined(TRACE)
			std::cerr << "source " << source.get_width() << "w x " << source.get_height() << "h\n"
				  << "target " << target.get_width() << "w x " << target.get_height() << "h\n";
#endif
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
#if defined(TRACE)
		std::cout << "risk " << std::fixed << risk << " (10 * " << incorrect_pixels << " + " << prefix.size() << ")\n";
#else
		std::cout << "risk " << std::fixed << risk << '\n';
#endif
	}
	catch (std::exception const& ex) {
		std::cerr << "Exception: " << ex.what() << ".\n";
		ret_val = 1;

	}

	return ret_val;
}
