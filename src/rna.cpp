#include <cmath>
#include <cstdlib>
#include <deque>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <sstream>

#include <zlib.h>
#include <png.h>

#include "rna.hpp"


// @TODO(bml)
void dump_state();


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

Bitmap transparentBitmap(BMP_HEIGHT, BMP_WIDTH);


Bucket bucket;
Pos position { 0, 0 };
Pos mark { 0, 0 };
Dir dir { Dir::E };
std::deque<Bitmap> bitmaps;
RNA rna;


// @TODO(bml)
void dump_state()
{
	std::cerr << "position { " << position.first << ", " << position.second << " }  ";
	std::cerr << "mark { " << mark.first << ", " << mark.second << " }  ";

	std::cerr << "dir ";
	switch (dir) {
	case Dir::N:  std::cerr << "North  ";  break;
	case Dir::E:  std::cerr << "East   ";   break;
	case Dir::S:  std::cerr << "South  ";  break;
	case Dir::W:  std::cerr << "West   ";   break;
	}

	std::cerr << "bucket:";
	if (bucket.empty()) {
		std::cerr << " <nil>\n";
	}
	else {
		for (const auto& color : bucket) {
			if (color.type == Color::Type::Color) {
				Component r, g, b;
				std::tie(r, g, b) = color.color;
			
				std::cerr << " { " << r << ' ' << g << ' ' << b << " }";
			}
			else {
				std::cerr << ' ' << color.alpha;
			}
		}
	}

	std::cerr << '\n';
}


void rna_init()
{
	transparentBitmap.fill(std::make_pair(std::make_tuple(0, 0, 0), 0));
	bitmaps.clear();
	bitmaps.push_back(transparentBitmap);
}


void build()
{
	// @TODO(BML) - give some idea of how far we've gotten
	static size_t counter = 0;
	// std::cerr << "initial state: ";
	// dump_state();

	for (const auto& r : rna) {
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
			// draw(bitmaps[0]);
			tryfill();
			// draw(bitmaps[0]);
		}
		else if (r == "PCCPFFP") {
			addBitmap(transparentBitmap);
		}
		else if (r == "PFFPCCP") {
			compose();
		}
		else if (r == "PFFICCF") {
			clip();
		}

		// @TODO(BML) - give some idea of how far we've gotten
		// std::cerr << "\nstep " << counter++ << ": " << r << "  ";
		// dump_state();
		counter++;
		if (!(counter % 8192))
			std::cerr << counter << " of " << rna.size() << " (" << (counter * 100 / rna.size()) << "%)\n";
	}

	draw(bitmaps[0]);
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

	for (const auto& color : bucket) {
		if (color.type == Color::Type::Color) {
			reds.push_back(std::get<0>(color.color));
			greens.push_back(std::get<1>(color.color));
			blues.push_back(std::get<2>(color.color));
		}
		else
			alphas.push_back(color.alpha);
	}

	auto red = average(reds, 0);
	auto green = average(greens, 0);
	auto blue = average(blues, 0);
	auto alpha = average(alphas, 255);

	double arange = static_cast<double>(alpha) / 255.0;

	auto new_r = static_cast<Component>(std::floor(static_cast<double>(red) * arange));
	auto new_g = static_cast<Component>(std::floor(static_cast<double>(green) * arange));
	auto new_b = static_cast<Component>(std::floor(static_cast<double>(blue) * arange));

	return std::make_pair(std::make_tuple(new_r, new_g, new_b), alpha);
}


Component average(const std::vector<Component>& values, Component def)
{
	if (values.empty()) {
		return def;
	}
	else {
		double sum = static_cast<double>(std::accumulate(std::begin(values), std::end(values), 0));
		double len = static_cast<double>(values.size());
		return static_cast<Component>(std::floor(sum / len));
	}
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
		d =  Dir::E;
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

	bitmaps[0].at(x, y) = currentPixel();
}


void line(Pos p0, Pos p1)
{
	Coord x0, y0, x1, y1;
	std::tie(x0, y0) = p0;
	std::tie(x1, y1) = p1;

	auto deltax = x1 - x0;
	auto deltay = y1 - y0;

	auto d = static_cast<double>(std::max(std::abs(deltax), std::abs(deltay)));

	double c = (deltax * deltay <= 0) ? 1.0 : 0.0;

	double x = static_cast<double>(x0) * d + std::floor((d - c) / 2.0);
	double y = static_cast<double>(y0) * d + std::floor((d - c) / 2.0);

	for (Coord t = 0; t < static_cast<Coord>(d); ++t) {
		setPixel(std::make_pair(static_cast<Coord>(std::floor(x / d)), static_cast<Coord>(std::floor(y / d))));
		x += deltax;
		y += deltay;
	}

	setPixel(std::make_pair(x1, y1));
}


void tryfill()
{
	auto n = currentPixel();
	auto o = getPixel(position);
	if (n != o)
		fill(position, o);
}


void fill(Pos p, Pixel initial)
{
	Component r, g, b;
	std::tie(r, g, b) = initial.first;
	auto a = initial.second;
	std::cerr << "fill({ " << p.first << ' ' << p.second << " }, { " << r << ' ' << g << ' ' << b << " | " << a << " })\n";
#if 0
	Pos::first_type x;
	Pos::second_type y;
	std::tie(x, y) = p;

	if (getPixel(p) == initial) {
		setPixel(p);
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
#else
	std::deque<Pos> work;
	work.push_back(p);

	while (!work.empty()) {
		auto node = work.front();
		work.pop_front();

		auto west = node;
		while (west.first > 0 && getPixel(west) == initial)
			west.first--;

		auto east = node;
		while (east.first < 599 && getPixel(east) == initial)
			east.first++;
		
		for (Pos pos = west; pos.second <= east.second; ++pos.second) {
			setPixel(pos);

			if (pos.second > 0) {
				auto north = std::make_pair(pos.first, pos.second - 1);
				if (getPixel(north) != initial)
					work.push_back(north);
			}
			
			if (pos.second < 599) {
				auto south = std::make_pair(pos.first, pos.second + 1);
				if (getPixel(south) != initial)
					work.push_back(south);
			}
		}
	}
#endif
}


void addBitmap(Bitmap& b)
{
	if (bitmaps.size() < 10) {
		bitmaps.push_front(b);
	}
	else {
		// @TODO(BML) - debug write bitmap that would be dumped
		// draw(b);
	}
}


void compose()
{
	if (bitmaps.size() >= 2) {
		for (int x = 0; x < 600; ++x) {
			for (int y = 0; y < 600; ++y) {
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

		// @TODO(bml) - write bitmap about to be lost
		// draw(bitmaps[0]);

		bitmaps.pop_front();
	}
}


void clip()
{
	if (bitmaps.size() >= 2) {
		for (int x = 0; x < 600; ++x) {
			for (int y = 0; y < 600; ++y) {
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

		// @TODO(BML) -- write bitmap about to be lost
		// draw(bitmaps[0]);

		bitmaps.pop_front();
	}
}


void draw(const Bitmap& bmp)
{
	static size_t counter = 0;

	std::stringstream ss;
	ss << "endo-" << std::setw(10) << std::setfill('0') << counter++ << ".pgm";
	std::ofstream pbm(ss.str());

	// header
	pbm << "P3\n" << bmp.cols() << ' ' << bmp.rows() << " 255\n";

	for (size_t row = 0; row < bmp.rows(); ++row) {
		for (size_t col = 0; col < bmp.cols(); ++col) {
			auto pixel = bmp.at(row, col);
			Component r, g, b;
			std::tie(r, g, b) = pixel.first;
			pbm << std::setw(4) << r << std::setw(4) << g << std::setw(4) << b << "  ";
		}

		pbm << '\n';
	}
}
