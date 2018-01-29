#pragma once

#include <deque>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

#include "array.hpp"

using Coord = int;
using Pos = std::pair<Coord, Coord>;

using Component = int;
using RGB = std::tuple<Component, Component, Component>;
using Transparency = Component;
using Pixel = std::pair<RGB, Transparency>;

constexpr size_t BMP_WIDTH = 600;
constexpr size_t BMP_HEIGHT = 600;

using Bitmap = bml::Array<Pixel>;

struct Color {
    enum class Type { Color,
        Transparency };
    Type type;
    RGB color;
    Transparency alpha;

    Color(RGB c)
        : type(Type::Color), color(c), alpha(0)
    {
    }
    Color(Transparency a)
        : type(Type::Transparency), color(std::make_tuple(0, 0, 0)), alpha(a)
    {
    }
};

using Bucket = std::deque<Color>;

enum class Dir { N,
    E,
    S,
    W };

using DNA = std::string;
using RNA = std::deque<DNA>;

extern const RGB black;
extern const RGB red;
extern const RGB green;
extern const RGB yellow;
extern const RGB blue;
extern const RGB magenta;
extern const RGB cyan;
extern const RGB white;

extern const Transparency transparent;
extern const Transparency opaque;

extern Bitmap transparentBitmap;

extern Bucket bucket;
extern Pos position;
extern Pos mark;
extern Dir dir;
extern std::deque<Bitmap> bitmaps; // <- transparentBitmap <| empty
extern RNA rna;

void rna_init();

void clip();
void compose();
void addBitmap(Bitmap& b);
void fill(Pos p, Pixel initial);
void tryfill();
void line(Pos p0, Pos p1);
void setPixel(Pos p);
Pixel getPixel(Pos p);
Dir turnClockwise(Dir d);
Dir turnCounterClockwise(Dir d);
Pos move(Pos p, Dir d);
Component average(const std::vector<Component>& values, Component def);
Pixel currentPixel();
void addColor(Color c);
void build();

void draw(const Bitmap& bmp);
