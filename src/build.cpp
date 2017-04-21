#include <cstdlib>
#include <deque>
#include <iostream>
#include <tuple>
#include <utility>
#include <vector>


using Coord = int;
using Pos = std::tuple<Coord, Coord>;

using Component = int;
using RGB = std::tuple<Component, Component, Component>;

using Transparency = Component;
using Pixel = std::tuple<RGB, Transparency>;

using Bitmap = Pixel[600][600];

using Color = RGB | Transparency;
using Bucket = std::vector<Color>;

using Dir = char; // N | E | S | W

const RGB black { 0, 0, 0 };
const RGB red { 255, 0, 0 };
const RGB green { 0, 255, 0 };
const RGB yellow { 255, 255, 0 };
const RGB blue { 0, 0, 255 };
const RGB magenta { 255, 0, 255 };
const RGB cyan { 0, 255, 255 };
const RGB white { 255, 255, 255 };

const Transparency transparent { 0 };
const Transparency opaque { 255 };

Bucket bucket;
Pos position { 0, 0 };
Pos mark { 0, 0 };
Dir dir { 'E' };
std::vector<Bitmap> bitmaps;  // <- transparentBitmap <| empty


void clip();
void compose();
void addBitmap(Bitmap b);
void fill(Pos p, Pixel initial);
void tryfill();
void line(Pos p0, Pos p1);
void setPixel(Pos p);
Pixel getPixel(Pos p);
Dir turnClockwise(Dir d);
Dir turnCounterClockwise(Dir d);
Pos move(Pos p, Dir d);
Component average(std::vector<Component> values, Component default);
Pixel currentPixel();
void addColor(Color c);
void build();




void build()
{
	rna = read();
	for (r : rna) {
		switch (r is of the form) {
		case 'PIPIIIC':
			addColor(black);
			break;

		case 'PIPIIIP':
			addColor(red);
			break;

		case 'PIPIICC':
			addColor(green);
			break;

		case 'PIPIICF':
			addColor(yellow);
			break;

		case 'PIPIICP':
			addColor(blue);
			break;

		case 'PIPIIFC':
			addColor(magenta);
			break;

		case 'PIPIIFF':
			addColor(cyan);
			break;

		case 'PIPIIPC':
			addColor(white);
			break;

		case 'PIPIIPF':
			addColor(transparent);
			break;

		case 'PIPIIPP':
			addColor(opaque);
			break;

		case 'PIIPICP':
			bucket.clear();
			break;

		case 'PIIIIIP':
			position = move(position, dir);
			break;

		case 'PCCCCCP':
			dir = turnCounterClockwise(dir);
			break;

		case 'PFFFFFP':
			dir = turnClockwise(dir);
			break;

		case 'PCCIFFP':
			mark = position;
			break;

		case 'PCCIFFP':
			line(position, mark);
			break;

		case 'PIIPIIP':
			tryfill();
			break;

		case 'PCCPFFP':
			addBitmap(transparentBitmap);
			break;

		case 'PFFPCCP':
			compose();
			break;

		case 'PFFICCF':
			clip();
			break;
		}
	}

	draw(bitmaps[0]);
}


void addColor(Color c)
{
	bucket.push_back(color);
}

Pixel currentPixel()
{
}

Component average(Component* values, Component default)
{
	if (values.empty()) {
		return default;
	}
	else {
		return std::floor(sum(values) / len(values));
	}
}


Pos move(Pos p, Dir d)
{
	Pos::first_type x;
	Pos::second_type y;
	std::tie(x, y) = p;
	
	switch (d) {
	case 'N':
		return std::make_pair(x,             (y - 1) % 600);

	case 'E':
		return std::make_pair((x + 1) % 600, y            );

	case 'S':
		return std::make_pair(x,             (y + 1) % 600);

	case 'W':
		return std::make_pair((x - 1) % 600, y            );

	default:
		std::cerr << "# !!! move d (" << d << ") is not N | E | S | W!\nBail out!\n";
		std::abort();
	}
}


Dir turnCounterClockwise(Dir d)
{
	switch (d) {
	case 'N':
		return 'W';

	case 'E':
		return 'N';

	case 'S':
		return 'E';

	case 'W':
		return 'W';

	default:
		std::cerr << "# !!! turnCounterClockwise d (" << d << ") is not N | E | S | W!\nBail out!\n";
		std::abort();
	}
}


Dir turnClockwise(Dir d)
{
	switch (d) {
	case 'N':
		return 'E';

	case 'E':
		return 'S';

	case 'S':
		return 'W';

	case 'W':
		return 'N';

	default:
		std::cerr << "# !!! turnClockwise d (" << d << ") is not N | E | S | W!\nBail out!\n";
		std::abort();
	}
}


Pixel getPixel(Pos p)
{
	std::tuple_element<0, Pos>::type x;
	std::tuple_element<1, Pos>::type y;
	std::tie(x, y) = p;

	return bitmaps[0][x][y];
}

void setPixel(Pos p)
{
	Pos::first_type x;
	Pos::second_type y;
	std::tie(x, y) = p;

	bitmaps[0][x][y] = currentPixel();
}

void line(Pos p0, Pos p1)
{
	Pos::first_type x0, x1;
	Pos::second_type y0, y1;
	std::tie(x0, y0) = p0;
	std::tie(x1, y1) = p1;
	
	auto deltax = x1 - x0;
	auto deltay = y1 - y0;

	auto d = std::max(std::abs(deltax), std::abs(deltay));

	int c;
	if (deltax * deltay <= 0) {
		c = 1;
	}
	else {
		c = 0;
	}

	auto x = x0 * d + std::floor((d - c) / 2);
	auto y = y0 * d + std::floor((d - c) / 2);

	for (int t = 0; t < d; ++t) {
		setPixel(std::make_pair(std::floor(x / d), std::floor(y / d)));
		x += deltax;
		y += deltay;
	}

	setPixel(std::make_pair(x1, y1));
}


void tryfill()
{
	auto _new = currentPixel();
	auto old = getPixel(position);
	if (new != old) {
		fill(position, old);
	}
}

void fill(Pos p, Pixel initial)
{
	Pos::first_type x;
	Pos::second_type y;
	std::tie(x, y) = p;

	if (getPixel(x, y) == initial) {
		setPixel(x, y);
		if (x > 0) {
			fill(std::make_pair(x - 1, y), initial);
		}
		if (x < 599) {
			fill(std::make_pair(x + 1, y), initial);
		}
		if (y > 0) {
			fill(std::make_pair(x, y - 1), initial);
		}
		if (y < 599) {
			fill(std::make_pair(x, y + 1), initial);
		}
	}
}


void addBitmap(Bitmap b)
{
	if (bitmaps.length() < 10) {
		bitmaps.push_front(b);
	}
}

void compose()
{
	if (bitmaps.length() >= 2) {
		for (int x = 0; x < 600; ++x) {
			for (int y = 0; y < 600; ++y) {
				let ((r0, g0, b0), a0) <- bitmaps[0][x][y];
				let ((r1, g1, b1), a1) <- bitmaps[1][x][y];

				auto r = r0 + std::floor(r1 * (255 - a0) / 255);
				auto g = g0 + std::floor(g1 * (255 - a0) / 255);
				auto b = b0 + std::floor(b1 * (255 - a0) / 255);
				auto a = a0 + std::floor(a1 * (255 - a0) / 255);
				
				bitmaps[1][x][y] = ((r, g, b), a);
			}
		}
	}
}


void clip()
{
	if (bitmaps.length() >= 2) {
		for (int x = 0; x < 600; ++x) {
			for (int y = 0; y < 600; ++y) {
				let ((r0, g0, b0), a0) <- bitmaps[0][x][y];
				let ((r1, g1, b1), a1) <- bitmaps[1][x][y];

				auto r = std::floor(r1 * a0 / 255);
				auto g = std::floor(g1 * a0 / 255);
				auto b = std::floor(b1 * a0 / 255);
				auto a = std::floor(a1 * a0 / 255);
				
				bitmaps[1][x][y] = ((r, g, b), a);
			}
		}

		bitmaps.pop_front();
	}
}


int main(int, char**)
{
	// @TODO - initialize globals

	build();

	return 0;
}
