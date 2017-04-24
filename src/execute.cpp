#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <iterator>
#include <string>
#include <map>
#include <deque>
#include <limits>


using Base = char;  // := I | C | F | P
using DNA = std::deque<Base>;
using RNA = std::deque<DNA>;


using Template = std::string;
using Pattern = std::string;
using Environment = std::map<int, DNA>;
using Number = int;


DNA dna;
RNA rna;


// inline bool dna_starts_with(const std::string& val) { return dna.find(val) == 0; }


DNA asnat(Number N);
DNA consts();
void finish();
void matchreplace(Pattern pat, Template t);
Number nat();
Pattern pattern();
DNA protect(Number l, DNA d);
DNA quote(DNA d);
void replace(Template tpl, Environment e);
Template templates();


DNA asnat(Number n)
{
#if 1
	DNA result;

	while (n) {
		if (n & 1) {
			result.push_back('C');
		}
		else {
			result.push_back('I');
		}

		n /= 2;
	}

	result.push_back('P');

	return result;
#else
	if (n == 0) {
		return DNA { 'P' };
	}
	else if (n & 1) {  // positive odd
		DNA left { 'C' };
		auto right = asnat(n / 2);
		left.insert(std::end(left), std::begin(right), std::end(right));
		return left;
	}
	else {             // positive odd
		DNA left { 'I' };
		auto right = asnat(n / 2);
		left.insert(std::end(left), std::begin(right), std::end(right));
		return left;
	}
#endif
}


// DNA consts()
// {
// 	DNA s;

// 	if (dna_starts_with("C")) {
// 		dna.erase(0, 1);
// 		s = consts();
// 		return "I" + s;
// 	}
// 	else if (dna_starts_with("F")) {
// 		dna.erase(0, 1);
// 		s = consts();
// 		return "C" + s;
// 	}
// 	else if (dna_starts_with("P")) {
// 		dna.erase(0, 1);
// 		s = consts();
// 		return "F" + s;
// 	}
// 	else if (dna_starts_with("IC")) {
// 		dna.erase(0, 2);
// 		s = consts();
// 		return "P" + s;
// 	}
// 	else {
// 		return DNA();
// 	}
// }


void finish()
{
	for (auto r : rna) {
		for (auto b : r)
			std::cout << b;
		std::cout << '\n';
	}

	exit(EXIT_SUCCESS);
}


// void matchreplace(Pattern pat, Template t)
// {
// 	Number i = 0;
// 	Environment e;
// 	std::vector<Number> c;

// 	for (size_t p = 0; p < pat.length(); ++p) {
// 		if (pat[p] == 'I' || pat[p] == 'C' || pat[p] == 'F' || pat[p] == 'P') {
// 			if (dna[0] == pat[p]) {
// 				i += 1;
// 			}
// 			else {
// 				return;
// 			}
// 		}
// 		else if (pat[p] == '!') {
// 			size_t len = 0;
// 			auto n = std::stoi(pat.substr(p), &len);
// 			p += len;  // skip the number
// 			i += n;
// 			if (n > dna.length()) {
// 				return;
// 			}
// 		}
// 		else if (pat[p] == '?') {
// 		}
// 		else if (pat[p] == ')') {
// 			c.push_front(i);
// 		}
// 		else if (pat[p] == ')') {
// 			e;
// 		}
// 	}
// }


Number nat()
{
	Number n = 0;

	if (dna.empty()) {
		std::cout << "#!!! nat() called with empty dna!\nBail out!\n";
		finish();
	}

#if 1
	if (dna[0] == 'P') {
		dna.pop_front();
		return 0;
	}

	auto end = std::find(std::begin(dna), std::end(dna), 'P');
	if (end == std::end(dna)) {
		std::cout << "# !!! nat() called without ending 'P'!\nBail out!\n";
		std::abort();
	}

	auto it = std::prev(end);
	while (it != std::begin(dna)) {
		switch (*it) {
		case 'I':
		case 'F':
			n *= 2;
			break;

		case 'C':
			n = 2 * n + 1;
			break;

		default:
			std::cout << "# !!! unrecognized base in DNA!\nBail out!\n";
			std::abort();
		}

		it = std::prev(it);
	}

	switch (*it) {
	case 'I':
	case 'F':
		n *= 2;
		break;

	case 'C':
		n = 2 * n + 1;
		break;

	default:
		std::cout << "# !!! unrecognized base in DNA part two!\nBail out!\n";
		std::abort();
	}

	return n;
#else
	switch (dna[0]) {
	case 'P':
		dna.pop_front();
		return 0;

	case 'I':
	case 'F':
		dna.pop_front();
		n = nat();
		return 2 * n;

	case 'C':
		dna.pop_front();
		n = nat();
		return 2 * n + 1;

	default:
		std::abort();
	}
#endif
}


// Pattern pattern()
// {
// 	Pattern p;
// 	int lvl = 0;

// 	while (true) {
// 		// std::cerr << "# DNA '" << dna << "' lvl " << lvl << '\n';

// 		if (dna_starts_with("C")) {
// 			dna.erase(0, 1);
// 			p += 'I';
// 			// std::cerr << "#  C - p '" << p << "'\n";
// 		}
// 		else if (dna_starts_with("F")) {
// 			dna.erase(0, 1);
// 			p += 'C';
// 			// std::cerr << "#  F - p " << p << '\n';
// 		}
// 		else if (dna_starts_with("P")) {
// 			dna.erase(0, 1);
// 			p += 'F';
// 			// std::cerr << "# P - p " << p << '\n';
// 		}
// 		else if (dna_starts_with("IC")) {
// 			dna.erase(0, 2);
// 			p += 'P';
// 			// std::cerr << "# IC - p " << p << '\n';
// 		}
// 		else if (dna_starts_with("IP")) {
// 			dna.erase(0, 2);
// 			auto n = nat();
// 			p += '!' + std::to_string(n);
// 			// std::cerr << "#  IP - p " << p << '\n';
// 		}
// 		else if (dna_starts_with("IF")) {
// 			dna.erase(0, 3);   // three bases consumed!
// 			auto s = consts();
// 			p += '?' + s;
// 			// std::cerr << "#  IF - p " << p << '\n';
// 		}
// 		else if (dna_starts_with("IIP")) {
// 			dna.erase(0, 3);
// 			lvl += 1;
// 			p += '(';
// 			// std::cerr << "#  IIP - p " << p << '\n';
// 		}
// 		else if (dna_starts_with("IIC") || dna_starts_with("IIF")) {
// 			dna.erase(0, 3);
// 			if (lvl == 0) {
// 				// std::cout << "# Result " << p << '\n';
// 				return p;
// 			}
// 			else {
// 				lvl -= 1;
// 				p += ')';
// 			}
// 			// std::cerr << "#  IIC/IIF - p " << p << '\n';
// 		}
// 		else if (dna_starts_with("III")) {
// 			rna += dna.substr(3, 10);
// 			dna.erase(0, 10);
// 			// std::cerr << "#  III\n";
// 		}
// 		else {
// 			std::cout << "# !!! pattern() Unrecognized DNA sequence!\nBail out!\n";
// 			finish();
// 		}
// 	}
// }


// DNA protect(Number l, DNA d)
// {
// 	if (l == 0) {
// 		return d;
// 	}
// 	else {
// 		return protect(l - 1, quote(d));
// 	}
// }


// DNA quote(DNA d)
// {
// 	switch (d[0]) {
// 	case 'I':
// 		return "C" + quote(d.substr(1));

// 	case 'C':
// 		return "F" + quote(d.substr(1));

// 	case 'F':
// 		return "P" + quote(d.substr(1));

// 	case 'P':
// 		return "IC" + quote(d.substr(1));

// 	default:
// 		return DNA();
// 	}
// }


// void replace(Template tpl, Environment e)
// {
// 	DNA r;
// 	for
// }


// Template templates()
// {
// 	Template t;

// 	while (true) {
// 		if (dna_starts_with("C")) {
// 			dna.erase(0, 1);
// 			t += 'I';
// 		}
// 		else if (dna_starts_with("F")) {
// 			dna.erase(0, 1);
// 			t += 'C';
// 		}
// 		else if (dna_starts_with("P")) {
// 			dna.erase(0, 1);
// 			t += 'F';
// 		}
// 		else if (dna_starts_with("IC")) {
// 			dna.erase(0, 2);
// 			t += 'P';
// 		}
// 		else if (dna_starts_with("IF") || dna_starts_with("IP")) {
// 			dna.erase(0, 2);
// 			auto l = nat();
// 			auto n = nat();
// 			t += std::to_string(n) + std::to_string(l);
// 		}
// 		else if (dna_starts_with("IIC") || dna_starts_with("IIF")) {
// 			dna.erase(0, 3);
// 			return t;
// 		}
// 		else if (dna_starts_with("IIP")) {
// 			dna.erase(0, 3);
// 			auto n = nat();
// 			t += '|' + std::to_string(n) + '|';
// 		}
// 		else if (dna_starts_with("III")) {
// 			rna += dna.substr(3, 10);
// 			dna.erase(0, 10);
// 		}
// 		else {
// 			std::cout << "# !!! tempates() Unrecognized DNA sequence!\nBail out!\n";
// 			finish();
// 		}
// 	}
// }


int main(int, char**)
{
	// Base b;
	// while (std::cin >> b)
	// 	dna.push_back(b);

	// while (true) {
	// 	auto p = pattern();
	// 	auto t = templates();
	// 	matchreplace(p, t);
	// }

	// test all positive integers
	std::cout << "1.." << std::numeric_limits<Number>::max() << '\n';
	for (Number expected = 0; expected < std::numeric_limits<Number>::max(); ++expected) {
		auto value = asnat(expected);

		dna = value;
		auto actual = nat();

#if defined(PROFILE)
		if (expected != actual) {
			std::cout << "not ok " << expected << " - ";
			for (auto v : value)
				std::cout << v;
			std::cout << " -> " << actual << '\n';
			std::abort();
		}
#else
		std::cout
			<< (expected != actual ? "not " : "") << "ok "
			<< expected << " - ";
		for (auto v : value)
			std::cout << v;
		std::cout << " -> " << actual << '\n';
#endif
	}

	return EXIT_SUCCESS;
}
