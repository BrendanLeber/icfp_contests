#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "build.hpp"

inline bool pixel_eq(const Pixel& left, const Pixel& right)
{
    return std::get<0>(left.first) == std::get<0>(right.first)
        && std::get<1>(left.first) == std::get<1>(right.first)
        && std::get<2>(left.first) == std::get<2>(right.first)
        && left.second == right.second;
}

inline std::string to_string(const Pixel& pixel)
{
    std::stringstream str;
    str << "((" << std::get<0>(pixel.first)
        << ", " << std::get<1>(pixel.first)
        << ", " << std::get<2>(pixel.first)
        << "), " << pixel.second
        << ")";
    return str.str();
}

int main()
{
    int retval = EXIT_SUCCESS;

    rna_init();

    std::cout << "1..4\n";

    {
        Pixel expected = std::make_pair(black, 170);

        bucket.clear();
        addColor(Color(transparent));
        addColor(Color(opaque));
        addColor(Color(opaque));

        auto actual = currentPixel();

        bool ok = pixel_eq(expected, actual);
        if (!ok) {
            retval = EXIT_FAILURE;
        }

        std::cout << (!ok ? "not " : "") << "ok 1"
                  << " - expected " << to_string(expected)
                  << " actual " << to_string(actual)
                  << '\n';
    }

    {
        Pixel expected = std::make_pair(std::make_tuple(85, 170, 85), 255);

        bucket.clear();
        addColor(Color(black));
        addColor(Color(yellow));
        addColor(Color(cyan));

        auto actual = currentPixel();

        bool ok = pixel_eq(expected, actual);
        if (!ok) {
            retval = EXIT_FAILURE;
        }

        std::cout << (!ok ? "not " : "") << "ok 2"
                  << " - expected " << to_string(expected)
                  << " actual " << to_string(actual)
                  << '\n';
    }

    {
        Pixel expected = std::make_pair(std::make_tuple(127, 127, 0), 127);

        bucket.clear();
        addColor(Color(yellow));
        addColor(Color(transparent));
        addColor(Color(opaque));

        auto actual = currentPixel();

        bool ok = pixel_eq(expected, actual);
        if (!ok) {
            retval = EXIT_FAILURE;
        }

        std::cout << (!ok ? "not " : "") << "ok 3"
                  << " - expected " << to_string(expected)
                  << " actual " << to_string(actual)
                  << '\n';
    }

    {
        Pixel expected = std::make_pair(std::make_tuple(143, 25, 125), 191);

        bucket.clear();
        for (auto i = 0; i < 18; ++i) {
		addColor(Color(black));
	}
        for (auto i = 0; i < 7; ++i) {
		addColor(Color(red));
	}
        for (auto i = 0; i < 39; ++i) {
		addColor(Color(magenta));
	}
        for (auto i = 0; i < 10; ++i) {
		addColor(Color(white));
	}
        for (auto i = 0; i < 3; ++i) {
		addColor(Color(opaque));
	}
        for (auto i = 0; i < 1; ++i) {
		addColor(Color(transparent));
	}

        auto actual = currentPixel();

        bool ok = pixel_eq(expected, actual);
        if (!ok) {
            retval = EXIT_FAILURE;
        }

        std::cout << (!ok ? "not " : "") << "ok 4"
                  << " - expected " << to_string(expected)
                  << " actual " << to_string(actual)
                  << '\n';
    }

    return retval;
}
