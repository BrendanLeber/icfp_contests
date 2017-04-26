#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <iterator>

#include "execute_functions.hpp"


DNA dna;
RNA rna;

#if defined(TRACE)

#include <sstream>

static std::string to_string(const DNA& dna);
static std::string to_string(const Pattern& pat);
static std::string to_string(const Template& templ);
static std::string to_string(const Environment& env);

static std::string
to_string(const DNA& dna)
{
	std::stringstream str;
	auto it = std::begin(dna);
	for (size_t p = 0; p < 10 && it != std::end(dna); ++it, ++p)
		if (it != std::end(dna))
			str << *it;
	if (it != std::end(dna))
		str << "...";
	return str.str();
}


static std::string
to_string(const Environment& env)
{
	std::stringstream str;
	if (env.empty()) {
		str << "e is empty";
	}
	else {
		size_t i = 0;
		for (const auto& e : env) {
			if (i > 0)
				str << '\n';

			str << "e[" << i++ << "] = ";

			auto it = std::begin(e);
			for (size_t j = 0; j < 10 && it != std::end(e); ++it, ++j)
				if (it != std::end(e))
					str << *it;
			if (it != std::end(e))
				str << "...";
			str << " (" << e.size() << " bases)";
		}
	}

	return str.str();
}

static std::string
to_string(const Pattern& pat)
{
	std::stringstream str;

	for (const auto& p : pat) {
		switch (p.type) {
		case PItem::Type::Base:
			str << p.base;
			break;

		case PItem::Type::Skip:
			str << '!' << p.skip;
			break;

		case PItem::Type::Search:
			str << '?';
			for (auto it = std::begin(p.search); it != std::end(p.search); ++it)
				str << *it;
			break;

		case PItem::Type::Open:
			str << '(';
			break;

		case PItem::Type::Close:
			str << ')';
			break;
		}
	}

	return str.str();
}

static std::string
to_string(const Template& templ)
{
	std::stringstream str;
	for (const auto& t : templ) {
		switch (t.type) {
		case TItem::Type::Base:
			str << t.base;
			break;

		case TItem::Type::Protection:
			str << '\\' << t.prot.second;
			break;

		case TItem::Type::Reference:
			str << '|' << t.ref << '|';
			break;
		}
	}

	return str.str();
}
#endif


static inline bool
dna_starts_with(const std::string& value)
{
	if (dna.empty() || value.empty())
		return false;

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


void execute()
{
	size_t iteration = -1;

	while (true) {
		++iteration;

#if defined(TRACE)
		std::cerr << "iteration " << iteration << '\n';
		std::cerr << "dna = " << to_string(dna) << " (" << dna.size() << " bases)\n";
#else
		if (!(iteration % 1000))
			std::cerr << "iteration " << iteration << '\n';
#endif

	 	auto pat = pattern();

#if defined(TRACE)
		std::cerr << "pattern " << to_string(pat) << '\n';
#endif

		auto templ = templates();

#if defined(TRACE)
		std::cerr << "template " << to_string(templ) << '\n';
#endif

		matchreplace(pat, templ);

#if defined(TRACE)
		std::cerr << "len(rna) = " << rna.size() << "\n\n";
#endif
	}
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


void matchreplace(const Pattern& pat, const Template& t)
{
	Number i = 0;
	Environment e;
	std::deque<Number> c;

	for (const auto& p : pat) {
		switch (p.type) {
		case PItem::Type::Base:
			if (dna[i] == p.base)
				i += 1;
			else {
#if defined(TRACE)
				std::cerr << "failed match\n";
#endif
				return;
			}
			break;

		case PItem::Type::Skip:
			i += p.skip;
			if (i > static_cast<Number>(dna.size())) {
#if defined(TRACE)
				std::cerr << "failed match\n";
#endif
				return;
			}
			break;

		case PItem::Type::Search:
		{
			auto start = std::begin(dna) + i;
			auto it = std::search(start, std::end(dna), std::begin(p.search), std::end(p.search));
			if (it != std::end(dna)) {
				i += std::distance(start, it) + p.search.size();
			}
			break;
		}

		case PItem::Type::Open:
			c.push_front(i);
			break;

		case PItem::Type::Close:
			e.push_back(DNA(std::begin(dna) + c.front(), std::begin(dna) + i));
			c.pop_front();
			break;
		}
	}

#if defined(TRACE)
	std::cerr << "successful match of length " << i << '\n';
#endif

	dna.erase(std::begin(dna), std::begin(dna) + i);
	replace(t, e);
}


Number nat()
{
	Number n = 0;

	if (dna.empty()) {
		finish();
	}

	if (dna[0] == 'P') {
		dna.pop_front();
		return 0;
	}

	auto end = std::find(std::begin(dna), std::end(dna), 'P');
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
			p.push_back(PItem('I'));
		}
		else if (dna_starts_with("F")) {
			dna.pop_front();
			p.push_back(PItem('C'));
		}
		else if (dna_starts_with("P")) {
			dna.pop_front();
			p.push_back(PItem('F'));
		}
		else if (dna_starts_with("IC")) {
			dna.erase(std::begin(dna), std::begin(dna) + 2);
			p.push_back(PItem('P'));
		}
		else if (dna_starts_with("IP")) {
			dna.erase(std::begin(dna), std::begin(dna) + 2);
			auto n = nat();
			p.push_back(PItem(n));
		}
		else if (dna_starts_with("IF")) {
			dna.erase(std::begin(dna), std::begin(dna) + 3);   // three bases consumed!
			auto s = consts();
			p.push_back(PItem(s));
		}
		else if (dna_starts_with("IIP")) {
			dna.erase(std::begin(dna), std::begin(dna) + 3);
			lvl += 1;
			p.push_back(PItem(true));  // open group
		}
		else if (dna_starts_with("IIC") || dna_starts_with("IIF")) {
			dna.erase(std::begin(dna), std::begin(dna) + 3);
			if (lvl == 0) {
				return p;
			}
			else {
				lvl -= 1;
				p.push_back(PItem(false));  // close group
			}
		}
		else if (dna_starts_with("III")) {
			rna.push_back(DNA(std::begin(dna) + 3, std::begin(dna) + 10));
			dna.erase(std::begin(dna), std::begin(dna) + 10);
		}
		else {
			std::cout << "# !!! pattern() Unrecognized DNA sequence!\nBail out!\n";
			finish();
		}
	}
}


DNA protect(Number l, const DNA& d)
{
	if (l == 0) {
		return d;
	}
	else {
		return protect(l - 1, quote(d));
	}
}


DNA quote(const DNA& d)
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


void replace(const Template& tpl, const  Environment& e)
{
	DNA p, r, n;

#if defined(TRACE)
	std::cerr << to_string(e) << '\n';
#endif

 	for (const auto& t : tpl) {
		switch (t.type) {
		case TItem::Type::Base:
			r.push_back(t.base);
			break;

		case TItem::Type::Protection:
			p = protect(t.prot.first, e[t.prot.second]);
			r.insert(std::end(r), std::begin(p), std::end(p));
			break;

		case TItem::Type::Reference:
			n = asnat(e[t.ref].size());
			r.insert(std::end(r), std::begin(n), std::end(n));
			break;
		}
	}

	dna.insert(std::begin(dna), std::begin(r), std::end(r));
}


Template templates()
{
	Template t;

	while (true) {
		if (dna_starts_with("C")) {
			dna.pop_front();
			t.push_back(TItem('I'));
		}
		else if (dna_starts_with("F")) {
			dna.pop_front();
			t.push_back(TItem('C'));
		}
		else if (dna_starts_with("P")) {
			dna.pop_front();
			t.push_back(TItem('F'));
		}
		else if (dna_starts_with("IC")) {
			dna.erase(std::begin(dna), std::begin(dna) + 2);
			t.push_back(TItem('P'));
		}
		else if (dna_starts_with("IF") || dna_starts_with("IP")) {
			dna.erase(std::begin(dna), std::begin(dna) + 2);
			auto l = nat();
			auto n = nat();
			t.push_back(TItem(l, n));
		}
		else if (dna_starts_with("IIC") || dna_starts_with("IIF")) {
			dna.erase(std::begin(dna), std::begin(dna) + 3);
			return t;
		}
		else if (dna_starts_with("IIP")) {
			dna.erase(std::begin(dna), std::begin(dna) + 3);
			auto n = nat();
			t.push_back((n));
		}
		else if (dna_starts_with("III")) {
			DNA slice(std::begin(dna) + 3, std::begin(dna) + 11);
			rna.push_back(slice);
			dna.erase(std::begin(dna), std::begin(dna) + 11);
		}
		else {
			std::cout << "# !!! tempates() Unrecognized DNA sequence!\nBail out!\n";
			finish();
		}
	}
}
