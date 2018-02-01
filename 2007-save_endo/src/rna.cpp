#include <cerrno>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <deque>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <numeric>
#include <sstream>
#include <stack>
#include <stdexcept>

#include "build.hpp"

const RGB black{ 0, 0, 0 }; /* NOLINT */
const RGB red{ 255, 0, 0 }; /* NOLINT */
const RGB green{ 0, 255, 0 }; /* NOLINT */
const RGB yellow{ 255, 255, 0 }; /* NOLINT */
const RGB blue{ 0, 0, 255 }; /* NOLINT */
const RGB magenta{ 255, 0, 255 }; /* NOLINT */
const RGB cyan{ 0, 255, 255 }; /* NOLINT */
const RGB white{ 255, 255, 255 }; /* NOLINT */

const Transparency transparent{ 0 }; /* NOLINT */
const Transparency opaque{ 255 }; /* NOLINT */

// @TODO(bml) Fix warnings.  For now bury them.
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wglobal-constructors"
#pragma clang diagnostic ignored "-Wexit-time-destructors"

Bucket bucket; /* NOLINT */

Pos position{ 0, 0 }; /* NOLINT */
Pos mark{ 0, 0 }; /* NOLINT */
Dir dir{ Dir::E };
std::deque<Bitmap> bitmaps; /* NOLINT */
RNA rna; /* NOLINT */
#pragma clang diagnostic pop

void internal_draw(const Bitmap& bmp);
void internal_fill(Pos const& p, Pixel const& initial, Pixel const& current);
void internal_setPixel(Pos const& p, Pixel const& pixel);
void internal_line(Pos p0, Pos p1, Pixel const& pixel);

void rna_init()
{
    bitmaps.clear();
    bitmaps.emplace_back(Bitmap {});
}

void addColor(Color c)
{
	bucket.push_back(c);
}

Pixel currentPixel()
{
	std::vector<Component> reds, greens, blues, alphas;

	reds.reserve(bucket.size());
	greens.reserve(bucket.size());
	blues.reserve(bucket.size());
	alphas.reserve(bucket.size());

	for (auto const& color : bucket) {
		if (color.type == Color::Type::Color) {
			reds.push_back(std::get<0>(color.color));
			greens.push_back(std::get<1>(color.color));
			blues.push_back(std::get<2>(color.color));
		}
		else {
			alphas.push_back(color.alpha);
		}
	}

	auto t_red = average(reds, 0);
	auto t_green = average(greens, 0);
	auto t_blue = average(blues, 0);
	auto t_alpha = average(alphas, 255);

	auto arange = static_cast<float>(t_alpha) / 255.0;

	auto new_r = static_cast<Component>(std::floor(static_cast<float>(t_red) * arange));
	auto new_g = static_cast<Component>(std::floor(static_cast<float>(t_green) * arange));
	auto new_b = static_cast<Component>(std::floor(static_cast<float>(t_blue) * arange));

	return std::make_pair(std::make_tuple(new_r, new_g, new_b), t_alpha);
}

Component average(const std::vector<Component>& values, Component def)
{
    if (values.empty()) {
        return def;
    }

    auto sum = static_cast<float>(std::accumulate(std::begin(values), std::end(values), 0.0));
    auto len = static_cast<float>(values.size());
    return static_cast<Component>(std::floor(sum / len));
}

static inline Coord mod(Coord a, Coord b)
{
    return (a % b + b) % b;
}

Pos move(Pos p, Dir d)
{
    Pos::first_type x;
    Pos::second_type y;
    std::tie(x, y) = p;

    switch (d) {
    case Dir::N:
        p = std::make_pair(x, mod(y - 1, 600));
        break;

    case Dir::E:
        p = std::make_pair(mod(x + 1, 600), y);
        break;

    case Dir::S:
        p = std::make_pair(x, mod(y + 1, 600));
        break;

    case Dir::W:
        p = std::make_pair(mod(x - 1, 600), y);
        break;
    }

    return p;
}

Dir turnCounterClockwise(Dir d)
{
    switch (d) {
    case Dir::N:
        d = Dir::W;
        break;

    case Dir::E:
        d = Dir::N;
        break;

    case Dir::S:
        d = Dir::E;
        break;

    case Dir::W:
        d = Dir::S;
        break;
    }

    return d;
}

Dir turnClockwise(Dir d)
{
    switch (d) {
    case Dir::N:
        d = Dir::E;
        break;

    case Dir::E:
        d = Dir::S;
        break;

    case Dir::S:
        d = Dir::W;
        break;

    case Dir::W:
        d = Dir::N;
        break;
    }

    return d;
}

Pixel getPixel(Pos p)
{
    Coord x, y;
    std::tie(x, y) = p;

    return bitmaps[0].at(x, y);
}

void setPixel(Pos p)
{
    Coord x, y;
    std::tie(x, y) = p;

    internal_setPixel(p, currentPixel());
}

void internal_setPixel(Pos const& p, Pixel const& pixel)
{
    Coord x, y;
    std::tie(x, y) = p;

    bitmaps[0].at(static_cast<size_t>(x), static_cast<size_t>(y)) = pixel;
}
	

void line(Pos p0, Pos p1)
{
#if defined(TRACE)
        std::cout << "line({" << p0.first << ' ' << p0.second << "}, {" << p1.first << ' ' << p1.second << "})\n";
#endif

	internal_line(p0, p1, currentPixel());
}

void internal_line(Pos p0, Pos p1, Pixel const& pixel)
{
    Coord x0, y0, x1, y1;
    std::tie(x0, y0) = p0;
    std::tie(x1, y1) = p1;

    auto deltax = x1 - x0;
    auto deltay = y1 - y0;

    auto d = static_cast<float>(std::max(std::abs(deltax), std::abs(deltay)));

    float c = (deltax * deltay <= 0) ? 1.0 : 0.0;

    auto x = static_cast<float>(x0) * d + std::floor((d - c) / 2.0);
    auto y = static_cast<float>(y0) * d + std::floor((d - c) / 2.0);

    for (Coord t = 0; t < static_cast<Coord>(d); ++t) {
	    internal_setPixel(
		    std::make_pair(static_cast<Coord>(std::floor(x / d)),
			static_cast<Coord>(std::floor(y / d))),
		    pixel);
        x += deltax;
        y += deltay;
    }

    internal_setPixel(std::make_pair(x1, y1), pixel);
}

void tryfill()
{
    auto n = currentPixel();
    auto o = getPixel(position);
    if (n != o) {
#if defined(SAVE_BITMAPS)
            internal_draw(bitmaps[0]);  // save what might be overwritten
#endif
        fill(position, o);
    }
}

void fill(Pos p, Pixel initial)
{
#if defined(TRACE)
        Component r, g, b;
        std::tie(r, g, b) = initial.first;
        auto a = initial.second;
        std::cout << "fill({ " << p.first << ' ' << p.second << " }, { " << r << ' ' << g << ' ' << b << " | " << a << " })\n";
#endif

    if (getPixel(p) == initial) {
	auto current = currentPixel();
	internal_fill(p, initial, current);
    }
}

void internal_fill(Pos const& p, Pixel const& initial, Pixel const& current)
{
    std::stack<Pos> work;
    work.emplace(p);

    while (!work.empty()) {
        auto node = work.top();
        work.pop();
	internal_setPixel(node, current);

	if (node.second > 0) {
		auto north = std::make_pair(node.first, node.second - 1);
		if (getPixel(north) == initial) {
			work.emplace(north);
		}
	}
	
        if (node.first < 599) {
		auto east = std::make_pair(node.first + 1, node.second);
		if (getPixel(east) == initial) {
			work.emplace(east);
		}
        }

	if (node.second < 599) {
		auto south = std::make_pair(node.first, node.second + 1);
		if (getPixel(south) == initial) {
			work.emplace(south);
		}
	}

	if (node.first > 0) {
		auto west = std::make_pair(node.first - 1, node.second);
		if (getPixel(west) == initial) {
			work.emplace(west);
		}
	}
    }
}

void addBitmap()
{
    if (bitmaps.size() < 10) {
	    bitmaps.emplace_front(Bitmap{});
    }
    else {
            std::cout << "not adding new transparent bitmap as our sequence is full.\n";
    }
}

void compose()
{
#if defined(TRACE)
        std::cout << "compose()\n";
#endif

#if defined(SAVE_BITMAPS)
	internal_draw(bitmaps[1]);
#endif

    if (bitmaps.size() >= 2) {
        for (size_t x = 0; x < 600; ++x) {
            for (size_t y = 0; y < 600; ++y) {
                // let ((r0, g0, b0), a0) <- bitmaps[0].at(x, y);
                auto const& p0 = bitmaps[0].at(x, y);
                Component r0, g0, b0;
                std::tie(r0, g0, b0) = p0.first;
                Transparency a0 = p0.second;

                // let ((r1, g1, b1), a1) <- bitmaps[1].at(x, y);
                auto const& p1 = bitmaps[1].at(x, y);
                Component r1, g1, b1;
                std::tie(r1, g1, b1) = p1.first;
                Transparency a1 = p1.second;

                auto r = r0 + std::floor(r1 * (255 - a0) / 255);
                auto g = g0 + std::floor(g1 * (255 - a0) / 255);
                auto b = b0 + std::floor(b1 * (255 - a0) / 255);
                auto a = a0 + std::floor(a1 * (255 - a0) / 255);

                bitmaps[1].at(x, y) = std::make_pair(std::make_tuple(r, g, b), a);
            }
        }

#if defined(SAVE_BITMAPS)
        internal_draw(bitmaps[0]);
#endif

        bitmaps.pop_front();
    }
}

void clip()
{
#if defined(TRACE)
        std::cout << "clip()\n";
#endif

#if defined(SAVE_BITMAPS)
	internal_draw(bitmaps[1]);
#endif

    if (bitmaps.size() >= 2) {
        for (size_t x = 0; x < 600; ++x) {
            for (size_t y = 0; y < 600; ++y) {
                // let ((r0, g0, b0), a0) <- bitmaps[0].at(x, y);
                auto const& p0 = bitmaps[0].at(x, y);
                Component r0, g0, b0;
                std::tie(r0, g0, b0) = p0.first;
                Transparency a0 = p0.second;

                // let ((r1, g1, b1), a1) <- bitmaps[1].at(x, y);
                auto const& p1 = bitmaps[1].at(x, y);
                Component r1, g1, b1;
                std::tie(r1, g1, b1) = p1.first;
                Transparency a1 = p1.second;

                auto r = std::floor(r1 * a0 / 255);
                auto g = std::floor(g1 * a0 / 255);
                auto b = std::floor(b1 * a0 / 255);
                auto a = std::floor(a1 * a0 / 255);

                bitmaps[1].at(x, y) = std::make_pair(std::make_tuple(r, g, b), a);
            }
        }

#if defined(SAVE_BITMAPS)
        internal_draw(bitmaps[0]);
#endif

        bitmaps.pop_front();
    }
}

void draw(Bitmap const& bmp)
{
#if defined(TRACE)
	std::cout << "draw()\n";
#endif
	internal_draw(bmp);
}

void internal_draw(Bitmap const& bmp)
{
#if defined(SAVE_BITMAPS)
        static size_t counter = 0;
        std::stringstream ss;
        ss << "endo-" << std::setw(10) << std::setfill('0') << counter++ << ".png";
#else
    std::stringstream ss;
    ss << "endo.png";
#endif

    bmp.write(ss.str());
}
