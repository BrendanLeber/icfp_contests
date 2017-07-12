#pragma once

#include <deque>
#include <string>
#include <utility>

using Base = char; // := I | C | F | P

using Number = int;

using DNA = std::deque<Base>;

using RNA = std::deque<DNA>;

DNA asnat(Number N);
DNA consts();
void execute();
void finish();
Number nat();

struct TItem {
    enum class Type { Base,
        Protection,
        Reference };
    Type type;
    union {
        Base base;
        std::pair<Number, Number> prot;
        Number ref;
    };

    TItem(Base b)
        : type(Type::Base), base(b)
    {
    }

    TItem(Number l, Number n)
        : type(Type::Protection), prot(std::make_pair(l, n))
    {
    }

    TItem(Number r)
        : type(Type::Reference), ref(r)
    {
    }
};

using Template = std::deque<TItem>;

struct PItem {
    enum class Type { Base,
        Skip,
        Search,
        Open,
        Close };
    Type type;

    Base base;
    Number skip;
    DNA search;
    bool open;

    PItem(Base b)
        : type(Type::Base), base(b), skip(0), search(DNA()), open(false)
    {
    }

    PItem(Number s)
        : type(Type::Skip), base(0), skip(s), search(DNA()), open(false)
    {
    }

    PItem(DNA s)
        : type(Type::Search), base(0), skip(0), search(s), open(false)
    {
    }

    PItem(bool op)
        : type(op ? Type::Open : Type::Close), base(0), skip(0), search(DNA()), open(op)
    {
    }

    ~PItem()
    {
        if (type == Type::Search)
            search.clear();
    }

    PItem(PItem&& src)
        : type(std::move(src.type)), base(std::move(src.base)), skip(std::move(src.skip)), search(std::move(src.search)), open(std::move(src.open))
    {
    }
};

using Pattern = std::deque<PItem>;

using Environment = std::deque<DNA>;

extern DNA dna;
extern RNA rna;

// inline bool dna_starts_with(const std::string& val) { return dna.find(val) == 0; }

void matchreplace(const Pattern& pat, const Template& t);
Pattern pattern();
DNA protect(Number l, const DNA& d);
DNA quote(const DNA& d);
void replace(const Template& tpl, const Environment& e);
Template templates();
