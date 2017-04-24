#pragma once

#include <string>
#include <map>
#include <deque>


using Base = char;  // := I | C | F | P
using DNA = std::deque<Base>;
using RNA = std::deque<DNA>;


using Template = std::string;
using Pattern = std::string;
using Environment = std::map<int, DNA>;
using Number = int;


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
