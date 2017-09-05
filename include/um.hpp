#pragma once

#include <array>
#include <cstdint>
#include <string>
#include <vector>

using Platter = uint32_t;
using Scroll = std::vector<Platter>;

struct Instruction {
	unsigned op;
	unsigned a;
	unsigned b;
	unsigned c;
	unsigned value;

	Instruction()
		: op(0), a(0), b(0), c(0), value(0)
	{
	}

	static Instruction decode(Platter word)
	{
		Instruction i;
		i.op = word >> 28;

		if (i.op == 0xD) {
			i.a = (word >> 25) & 7;
			i.value = word & 0177777777;
		}
		else {
			i.a = (word >> 6) & 7;
			i.b = (word >> 3) & 7;
			i.c = word & 7;
		}

		return i;
	}
};

struct Universal_Machine {
	std::array<Platter, 8> regs;
	Scroll zero;
	size_t ip;

	void initialize(Scroll& start);
	bool spin();
};

Scroll read_scroll(const std::string& fname);
