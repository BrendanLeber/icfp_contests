#include <cerrno>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <deque>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <locale>
#include <numeric>
#include <sstream>
#include <stack>
#include <stdexcept>
#include <utility>
#include <vector>

#if defined(COUNT_UNHANDLED_RNA)
#include <map>
#endif

#include "bitmap.hpp"

using Coord = int;
using Pos = std::pair<Coord, Coord>;

using Component = int;
using RGB = std::tuple<Component, Component, Component>;
using Transparency = Component;
using Pixel = std::pair<RGB, Transparency>;

using Bitmap = Array<Pixel>;

struct Color {
    enum class Type { Color,
        Transparency };
    Type type;
    RGB color;
    Transparency alpha;

    explicit Color(RGB c)
        : type(Type::Color), color(std::move(c)), alpha(0)
    {
    }

    explicit Color(Transparency a)
        : type(Type::Transparency), color(std::make_tuple(0, 0, 0)), alpha(a)
    {
    }
};

using Bucket = std::vector<Color>;

enum class Direction { N,
    E,
    S,
    W };

static Bucket bucket;
static Pos position{ 0, 0 }; /* NOLINT */
static Pos mark{ 0, 0 }; /* NOLINT */
static Direction dir{ Direction::E };
static std::deque<Bitmap> bitmaps; /* NOLINT */

Component average(std::vector<Component> const& values, Component def);

void clip();
void compose();
Pixel current_pixel();
void fill(Pos p, Pixel initial);
void fill(Pos p, Pixel initial, Pixel current);
Pixel get_pixel(Pos p);
void line(Pos p0, Pos p1);
void line(Pos p0, Pos p1, Pixel pixel);
Pos move(Pos p, Direction d);
void set_pixel(Pos p);
void set_pixel(Pos p, Pixel pixel);
void tryfill();

#if defined(SAVE_BITMAPS)
void save_bitmaps(Bitmap const& bmp);
#endif

Component average(std::vector<Component> const& values, Component def)
{
    if (values.empty()) {
        return def;
    }

    auto sum = static_cast<float>(std::accumulate(std::begin(values), std::end(values), 0));
    auto len = static_cast<float>(values.size());
    return static_cast<Component>(std::floor(sum / len));
}

void clip()
{
#if defined(TRACE)
    std::cout << "clip()\n";
#endif

#if defined(SAVE_BITMAPS)
    save_bitmaps(bitmaps[1]);
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
        save_bitmaps(bitmaps[0]);
#endif

        bitmaps.pop_front();
    }
}

void compose()
{
#if defined(TRACE)
    std::cout << "compose()\n";
#endif

#if defined(SAVE_BITMAPS)
    if (bitmaps.size() > 1) {
        save_bitmaps(bitmaps[1]);
    }
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
        save_bitmaps(bitmaps[0]);
#endif

        bitmaps.pop_front();
    }
}

Pixel current_pixel()
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

void fill(Pos p, Pixel initial)
{
#if defined(TRACE)
    Component r, g, b;
    std::tie(r, g, b) = initial.first;
    auto a = initial.second;
    std::cout << "fill({ " << p.first << ' ' << p.second << " }, { " << r << ' ' << g << ' ' << b << " | " << a << " })\n";
#endif

    if (get_pixel(p) == initial) {
        auto current = current_pixel();
        fill(p, initial, current);
    }
}

void fill(Pos p, Pixel initial, Pixel current)
{
    std::stack<Pos> work;
    work.emplace(p);

    while (!work.empty()) {
        auto node = work.top();
        work.pop();
        set_pixel(node, current);

        if (node.second > 0) {
            auto north = std::make_pair(node.first, node.second - 1);
            if (get_pixel(north) == initial) {
                work.emplace(north);
            }
        }

        if (node.first < 599) {
            auto east = std::make_pair(node.first + 1, node.second);
            if (get_pixel(east) == initial) {
                work.emplace(east);
            }
        }

        if (node.second < 599) {
            auto south = std::make_pair(node.first, node.second + 1);
            if (get_pixel(south) == initial) {
                work.emplace(south);
            }
        }

        if (node.first > 0) {
            auto west = std::make_pair(node.first - 1, node.second);
            if (get_pixel(west) == initial) {
                work.emplace(west);
            }
        }
    }
}

Pixel get_pixel(Pos p)
{
    Coord x, y;
    std::tie(x, y) = p;

    return bitmaps[0].at(x, y);
}

void line(Pos p0, Pos p1)
{
#if defined(TRACE)
    std::cout << "line({" << p0.first << ' ' << p0.second << "}, {" << p1.first << ' ' << p1.second << "})\n";
#endif

    line(p0, p1, current_pixel());
}

void line(Pos p0, Pos p1, Pixel pixel)
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
        set_pixel(
            std::make_pair(static_cast<Coord>(std::floor(x / d)),
                static_cast<Coord>(std::floor(y / d))),
            pixel);
        x += deltax;
        y += deltay;
    }

    set_pixel(std::make_pair(x1, y1), pixel);
}

Coord mod(Coord a, Coord b)
{
    return (a % b + b) % b;
}

Pos move(Pos p, Direction d)
{
    Pos::first_type x;
    Pos::second_type y;
    std::tie(x, y) = p;

    switch (d) {
    case Direction::N:
        p = std::make_pair(x, mod(y - 1, 600));
        break;

    case Direction::E:
        p = std::make_pair(mod(x + 1, 600), y);
        break;

    case Direction::S:
        p = std::make_pair(x, mod(y + 1, 600));
        break;

    case Direction::W:
        p = std::make_pair(mod(x - 1, 600), y);
        break;
    }

    return p;
}

#if defined(SAVE_BITMAPS)
void save_bitmaps(Bitmap const& bmp)
{
    static size_t counter = 0;
    std::stringstream ss;
    ss << "endo-" << std::setw(10) << std::setfill('0') << counter++ << ".png";
    bmp.write(ss.str());
}
#endif

void set_pixel(Pos p)
{
    Coord x, y;
    std::tie(x, y) = p;

    set_pixel(p, current_pixel());
}

void set_pixel(Pos p, Pixel pixel)
{
    Coord x, y;
    std::tie(x, y) = p;

    bitmaps[0].at(static_cast<size_t>(x), static_cast<size_t>(y)) = pixel;
}

void tryfill()
{
    auto n = current_pixel();
    auto o = get_pixel(position);
    if (n != o) {
#if defined(SAVE_BITMAPS)
        save_bitmaps(bitmaps[0]); // save what might be overwritten
#endif
        fill(position, o);
    }
}

int main(int argc, char** argv)
{
    if (argc != 2) {
        std::cerr << "Syntax: " << argv[0] << " <rna-file>\n"; /* NOLINT */
        exit(EXIT_FAILURE);
    }

    bitmaps.emplace_back(Bitmap{});

#if defined(COUNT_UNHANDLED_RNA)
    std::map<std::string, unsigned> unhandled_rna;
#endif

    std::cout.imbue(std::locale(""));

    try {
        // @TODO(BML) - Read from stdin if argv[1] == "-"
        std::ifstream in(argv[1]); /* NOLINT */

        static size_t counter = 0;
        std::cout << "rna " << std::fixed << counter << '\n';

        std::string rna;
        while (std::getline(in, rna)) {
            ++counter;
            if ((counter % 1024) == 0) {
                std::cout << "rna " << std::fixed << counter << '\n';
            }

            if (rna == "PIPIIIC") {
                // black
                bucket.emplace_back(Color{ RGB{ 0, 0, 0 } });
            }
            else if (rna == "PIPIIIP") {
                // red
                bucket.emplace_back(Color{ RGB{ 255, 0, 0 } });
            }
            else if (rna == "PIPIICC") {
                // green
                bucket.emplace_back(Color{ RGB{ 0, 255, 0 } });
            }
            else if (rna == "PIPIICF") {
                // yellow
                bucket.emplace_back(Color{ RGB{ 255, 255, 0 } });
            }
            else if (rna == "PIPIICP") {
                // blue
                bucket.emplace_back(Color{ RGB{ 0, 0, 255 } });
            }
            else if (rna == "PIPIIFC") {
                // magenta
                bucket.emplace_back(Color{ RGB{ 255, 0, 255 } });
            }
            else if (rna == "PIPIIFF") {
                // cyan
                bucket.emplace_back(Color{ RGB{ 0, 255, 255 } });
            }
            else if (rna == "PIPIIPC") {
                // white
                bucket.emplace_back(Color{ RGB{ 255, 255, 255 } });
            }
            else if (rna == "PIPIIPF") {
                // transparent
                bucket.emplace_back(Color{ Transparency{ 0 } });
            }
            else if (rna == "PIPIIPP") {
                // opaque
                bucket.emplace_back(Color{ Transparency{ 255 } });
            }
            else if (rna == "PIIPICP") {
                bucket.clear();
            }
            else if (rna == "PIIIIIP") {
                position = move(position, dir);
            }
            else if (rna == "PCCCCCP") {
                switch (dir) {
                case Direction::N:
                    dir = Direction::W;
                    break;

                case Direction::W:
                    dir = Direction::S;
                    break;

                case Direction::S:
                    dir = Direction::E;
                    break;

                case Direction::E:
                    dir = Direction::N;
                    break;
                }
            }
            else if (rna == "PFFFFFP") {
                switch (dir) {
                case Direction::N:
                    dir = Direction::E;
                    break;

                case Direction::E:
                    dir = Direction::S;
                    break;

                case Direction::S:
                    dir = Direction::W;
                    break;

                case Direction::W:
                    dir = Direction::N;
                    break;
                }
            }
            else if (rna == "PCCIFFP") {
                mark = position;
            }
            else if (rna == "PFFICCP") {
                line(position, mark);
            }
            else if (rna == "PIIPIIP") {
                tryfill();
            }
            else if (rna == "PCCPFFP") {
                if (bitmaps.size() < 10) {
                    bitmaps.emplace_front(Bitmap{});
                }
            }
            else if (rna == "PFFPCCP") {
                compose();
            }
            else if (rna == "PFFICCF") {
                clip();
            }
#if defined(COUNT_UNHANDLED_RNA)
            else {
                auto it = unhandled_rna.find(rna);
                if (it == std::end(unhandled_rna)) {
                    unhandled_rna.emplace(std::make_pair(rna, 1));
                }
                else {
                    it->second++;
                }
            }
#endif
        }

#if defined(TRACE)
        std::cout << "draw bitmaps[0]\n";
#endif
        // <- transparentBitmap <| empty
        bitmaps[0].write("endo.png");

        // how many RNA sequences did we process?
        std::cout << "rna " << std::fixed << counter << '\n';
    }
    catch (std::exception const& ex) {
        std::cerr << "exception!!! " << ex.what() << '\n';
    }

#if defined(COUNT_UNHANDLED_RNA)
    unsigned total_unused = 0;
    std::cout << "Unhandled RNA Counts:\n";
    for (auto const& kv : unhandled_rna) {
        total_unused += kv.second;
        std::cout << "  " << kv.first << ' ' << std::fixed << kv.second << '\n';
    }
    std::cout << " total " << total_unused << '\n';
#endif

    return EXIT_SUCCESS;
}
