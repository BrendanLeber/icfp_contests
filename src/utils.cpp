#include <cstdio>
#include <cstdlib>
#include <sstream>

#include "utils.hpp"


Scroll load(const std::string& fname)
{
	auto in = std::fopen(fname.c_str(), "rb");
	if (!in) {
		std::perror("File open failed.");
		std::exit(EXIT_FAILURE);
	}

	std::fseek(in, 0, SEEK_END);
	size_t platters = std::ftell(in) / sizeof(Platter);
	std::fseek(in, 0, SEEK_SET);

	Scroll scroll(platters);
	auto cnt = std::fread(scroll.data(), sizeof(Platter), scroll.size(), in);
	if (cnt != scroll.size()) {
		std::exit(EXIT_FAILURE);
	}

	std::fclose(in);

	return scroll;		
}

std::string decode(Platter platter)
{
	std::stringstream ss;

	auto op = platter >> 28;
	auto a = (platter >> 6) & 7;
	auto b = (platter >> 3) & 7;
	auto c = platter & 7;

	switch (op) {
	case 0:
		ss << "cmov r" << a << ", r" << b;
		break;

	case 1:
		ss << "r" << a << " = r" << b << '[' << c << ']';
		break;
		x
		
