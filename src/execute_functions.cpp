#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <iterator>

#include "execute_functions.hpp"


DNA dna;
RNA rna;


static inline bool
dna_starts_with(const std::string& value)
{
	auto it = std::begin(dna);
	for (auto base : value) {
		if (base != *it)
			return false;
		++it;
	}

	return true;
}


DNA asnat(Number n)
{
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
}


DNA consts()
{
	DNA s;

	if (dna_starts_with("C")) {
		dna.pop_front();
		s = consts();
		s.push_front('I');
	}
	else if (dna_starts_with("F")) {
		dna.pop_front();
		s = consts();
		s.push_front('C');
	}
	else if (dna_starts_with("P")) {
		dna.pop_front();
		s = consts();
		s.push_front('F');
	}
	else if (dna_starts_with("IC")) {
		dna.erase(std::begin(dna), std::begin(dna) + 2);
		s = consts();
		s.push_front('P');
	}
	// else fall through with no value in s

	return s;
}


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

	dna.erase(std::begin(dna), end + 1);

	return n;
}


Pattern pattern()
{
	Pattern p;
	int lvl = 0;

	while (true) {
		if (dna_starts_with("C")) {
			dna.pop_front();
			p += 'I';
		}
		else if (dna_starts_with("F")) {
			dna.pop_front();
			p += 'C';
		}
		else if (dna_starts_with("P")) {
			dna.pop_front();
			p += 'F';
		}
		else if (dna_starts_with("IC")) {
			dna.erase(std::begin(dna), std::begin(dna) + 2);
			p += 'P';
		}
		else if (dna_starts_with("IP")) {
			dna.erase(std::begin(dna), std::begin(dna) + 2);
			auto n = nat();
			p += '!' + std::to_string(n);
		}
		else if (dna_starts_with("IF")) {
			dna.erase(std::begin(dna), std::begin(dna) + 3);   // three bases consumed!
			auto s = consts();
			p.push_back('?');
			p.append(std::begin(s), std::end(s));
		}
		else if (dna_starts_with("IIP")) {
			dna.erase(std::begin(dna), std::begin(dna) + 3);
			lvl += 1;
			p += '(';
		}
		else if (dna_starts_with("IIC") || dna_starts_with("IIF")) {
			dna.erase(std::begin(dna), std::begin(dna) + 3);
			if (lvl == 0) {
				return p;
			}
			else {
				lvl -= 1;
				p += ')';
			}
		}
		else if (dna_starts_with("III")) {
			rna.push_back(DNA(std::begin(dna) + 3, std::begin(dna) + 11));
			dna.erase(std::begin(dna), std::begin(dna) + 11);
		}
		else {
			std::cout << "# !!! pattern() Unrecognized DNA sequence!\nBail out!\n";
			finish();
		}
	}
}


// DNA protect(Number l, DNA d)
// {
// 	if (l == 0) {
// 		return d;
// 	}
// 	else {
// 		return protect(l - 1, quote(d));
// 	}
// }


DNA quote(DNA d)
{
	DNA result;

	if (!d.empty()) {
		switch (d.front()) {
		case 'I':
			result = quote(DNA(std::begin(d) + 1, std::end(d)));
			result.push_front('C');
			break;

		case 'C':
			result = quote(DNA(std::begin(d) + 1, std::end(d)));
			result.push_front('F');
			break;

		case 'F':
			result = quote(DNA(std::begin(d) + 1, std::end(d)));
			result.push_front('P');
			break;

		case 'P':
			result = quote(DNA(std::begin(d) + 1, std::end(d)));
			result.push_front('C');
			result.push_front('I');
			break;
		}
	}

	return result;
}


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
// 			dna.pop_front();
// 			t += 'I';
// 		}
// 		else if (dna_starts_with("F")) {
// 			dna.pop_front();
// 			t += 'C';
// 		}
// 		else if (dna_starts_with("P")) {
// 			dna.pop_front();
// 			t += 'F';
// 		}
// 		else if (dna_starts_with("IC")) {
// 			dna.pop_front();
// 			t += 'P';
// 		}
// 		else if (dna_starts_with("IF") || dna_starts_with("IP")) {
// 			dna.erase(std::begin(dna), std::begin(dna) + 2);
// 			auto l = nat();
// 			auto n = nat();
// 			t += '\\' + std::to_string(n) + '\\' + std::to_string(l);
// 		}
// 		else if (dna_starts_with("IIC") || dna_starts_with("IIF")) {
// 			dna.erase(std::begin(dna), std::begin(dna) + 3);
// 			return t;
// 		}
// 		else if (dna_starts_with("IIP")) {
// 			dna.erase(std::begin(dna), std::begin(dna) + 3);
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
