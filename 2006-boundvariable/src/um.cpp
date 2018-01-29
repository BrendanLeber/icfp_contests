#include <cerrno>
#include <fstream>
#include <system_error>

#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "um.hpp"

Scroll read_scroll(const std::string& fname)
{
	std::ifstream inf(fname, std::ios::binary);
	if (!inf.is_open()) {
		throw std::runtime_error("unable to open input scroll");
	}

    struct stat sb = {};
    if (stat(fname.c_str(), &sb) == -1) {
	    throw std::system_error(errno, std::system_category());
    }

    auto num_platters = static_cast<size_t>(sb.st_size) / sizeof(std::uint32_t);

    Scroll scroll;
    scroll.reserve(num_platters);

    char ch;
    size_t n = 4;
    Platter platter { 0 };
    while (!inf.eof()) {
	    inf.get(ch);
	    if (!n--) {
		    n = 3;
		    // platter = ntohl(platter);
		    scroll.push_back(platter);
	    }
	    platter <<= 8;
	    platter |= static_cast<uint8_t>(ch);
    }

    return scroll;
}


void Universal_Machine::initialize(Scroll& start)
{
	std::fill(std::begin(regs), std::end(regs));
	ip = 0;
	zero = start;
}

bool Universal_Machine::spin()
{
	bool halt = false;

	auto inst = Instruction::decode(zero[ip]);

	if (inst.op == 0) {
		if (regs[inst.c]) {
			regs[inst.a] = regs[inst.b];
		}
	}
	else if (inst.op == 1) {
		regs[inst.a] = arr(regs[inst.b])[regs[inst.c]];
	}
	else if (inst.op == 2) {
		arr(regs[inst.a])[regs[inst.b]] = regs[inst.c];
	}
	else if (inst.op == 3) {
		regs[inst.a] = regs[inst.b] * regs[inst.c];
	}
	else if (inst.op == 4) {
		regs[inst.a] = regs[inst.b] * regs[inst.c];
	}
	else if (inst.op == 5) {
		regs[inst.a] = regs[inst.b] / regs[inst.c];
	}
	else if (inst.op == 6) {
		regs[inst.a] = ~(regs[inst.b] & regs[inst.c]);
	}

	return halt;
}
