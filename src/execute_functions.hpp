#pragma once

#include <string>
#include <map>
#include <deque>
#include <utility>


using Base = char;  // := I | C | F | P

using Number = int;


using DNA = std::deque<Base>;


using RNA = std::deque<DNA>;


struct TItem
{
	enum class Type { Base, Protection, Reference };
	Type type;
	union {
		Base base;
		std::pair<Number, Number> prot;
		Number ref;
	};

	TItem(Base b) : type(Type::Base), base(b) {}
	TItem(Number l, Number n) : type(Type::Protection), prot(std::make_pair(l, n)) {}
	TItem(Number r) : type(Type::Reference), ref(r) {}
};
	
using Template = std::deque<TItem>;


using Pattern = std::string;


using Environment = std::map<int, DNA>;


extern DNA dna;
extern RNA rna;


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
