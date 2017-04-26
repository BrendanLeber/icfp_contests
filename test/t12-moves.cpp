#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "rna.hpp"


inline bool eq(const Pos& left, const Pos& right)
{
	return left.first == right.first && left.second == right.second;
}


inline std::string to_string(const Pos& pos)
{
	std::stringstream str;
	str << "[ " << pos.first << ", " << pos.second << " ]";
							  return str.str();
}

inline std::string to_string(const Dir& dir)
{
	std::stringstream str;
	switch (dir) {
	case Dir::N:
		str << 'N';
		break;

	case Dir::E:
		str << 'E';
		break;

	case Dir::S:
		str << 'S';
		break;

	case Dir::W:
		str << 'W';
		break;
	}

	return str.str();
}

int main(int, char**)
{
	rna_init();

	std::vector<std::pair<Dir, Dir>> ccw_tests {
		{ Dir::N, Dir::W },
		{ Dir::E, Dir::N },
		{ Dir::S, Dir::E },
		{ Dir::W, Dir::S }
	};

	std::vector<std::pair<Dir, Dir>> cw_tests {
		{ Dir::N, Dir::E },
		{ Dir::E, Dir::S },
		{ Dir::S, Dir::W },
		{ Dir::W, Dir::N }
	};

	std::vector<std::pair<std::pair<Pos, Dir>, Pos>> moves {
		{ { { 300, 300 }, Dir::N }, { 300, 299 } },
		{ { { 300, 300 }, Dir::E }, { 301, 300 } },
		{ { { 300, 300 }, Dir::S }, { 300, 301 } },
		{ { { 300, 300 }, Dir::W }, { 299, 300 } },
		{ { {   0,   0 }, Dir::N }, {   0, 599 } },
		{ { {   0,   0 }, Dir::W }, { 599,   0 } },
		{ { { 599, 599 }, Dir::S }, { 599,   0 } },
		{ { { 599, 599 }, Dir::E }, {   0, 599 } }
	};

	std::cout << "1.." << ccw_tests.size() + cw_tests.size() + moves.size() << '\n';
	size_t test = 0;

	for (const auto& ccw : ccw_tests) {
		auto actual = turnCounterClockwise(ccw.first);
		auto ok = actual == ccw.second;

		std::cout << (!ok ? "not " : "") << "ok " << ++test
			  << " - ccw " << to_string(ccw.first) << " -> "
			  << to_string(actual) << " expected "
			  << to_string(ccw.second) << '\n';
	}

	for (const auto& cw : cw_tests) {
		auto actual = turnClockwise(cw.first);
		auto ok = actual == cw.second;

		std::cout << (!ok ? "not " : "") << "ok " << ++test
			  << " - cw " << to_string(cw.first) << " -> "
			  << to_string(actual) << " expected "
			  << to_string(cw.second) << '\n';
	}

	for (const auto& mv : moves) {
		auto actual = move(mv.first.first, mv.first.second);
		auto ok = eq(actual, mv.second);
		std::cout << (!ok ? "not " : "") << "ok " << ++test
			  << " - move(" << to_string(mv.first.first) << ", " << to_string(mv.first.second)
			  << ") -> " << to_string(actual)
			  << " expected " << to_string(mv.second) << '\n';
	}

	return EXIT_SUCCESS;
}
