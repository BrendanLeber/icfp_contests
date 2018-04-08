#pragma once

#include "rope.hpp"
#include <cctype>
#include <deque>
#include <fstream>
#include <iostream>
#include <string>
#include <utility>

using Base = char; // := I | C | F | P

using Number = int;

using DNA = Rope;

struct TItem {
    enum class Type { Base,
        Protection,
        Reference };
    Type type;

    Base base;
    std::pair<Number, Number> prot;
    Number ref;

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
            search.erase();
    }

    PItem(PItem&& src)
        : type(std::move(src.type)), base(std::move(src.base)), skip(std::move(src.skip)), search(std::move(src.search)), open(std::move(src.open))
    {
    }
};

using Pattern = std::deque<PItem>;
using Environment = std::deque<DNA>;

struct Arrow {
    size_t iteration{ 0 };
    size_t cost{ 0 };
    size_t rna{ 0 };
    DNA dna;

    static DNA asnat(Number n)
    {
        DNA result;

        while (n > 0) {
            if ((n & 1) != 0) {
                result.append("C");
            }
            else {
                result.append("I");
            }

            n /= 2;
        }

        result.append("P");

        return result;
    }

    DNA consts()
    {
        DNA s;

        if (dna_starts_with("C")) {
            dna.erase(0, 1);
            s = consts();
            s.prepend("I");
        }
        else if (dna_starts_with("F")) {
            dna.erase(0, 1);
            s = consts();
            s.prepend("C");
        }
        else if (dna_starts_with("P")) {
            dna.erase(0, 1);
            s = consts();
            s.prepend("F");
        }
        else if (dna_starts_with("IC")) {
            dna.erase(0, 2);
            s = consts();
            s.prepend("P");
        }
        // else fall through with no value in s

        return s;
    }

    bool dna_starts_with(std::string const& value)
    {
        if (dna.empty() || value.empty()) {
            return false;
        }

        for (size_t pos = 0; pos < value.size(); ++pos) {
            if (dna.at(pos) != value[pos]) {
                return false;
            }
        }

        return true;
    }

    void execute()
    {
#if defined(TRACE)
        std::cerr << "iteration " << iteration << '\n';
        std::cerr << "dna = " << to_string(dna) << '\n';
#else
        if ((iteration % 1024u) == 0) {
            std::cerr << "iteration " << std::fixed << iteration << "  dna " << std::fixed << dna.length() << "  rna " << std::fixed << rna << "  cost " << std::fixed << cost << '\n';
        }
#endif

        ++iteration;

        auto pat = pattern();

#if defined(TRACE)
        std::cerr << "pattern  " << to_string(pat) << '\n';
#endif

        auto templ = templates();

#if defined(TRACE)
        std::cerr << "template " << to_string(templ) << '\n';
#endif

        matchreplace(pat, templ);

#if defined(TRACE)
        std::cerr << "len(rna) = " << rna << "\n\n";
#endif

        if (!dna.is_balanced()) {
            dna.balance();
        }
    }

    void finish()
    {
        std::cerr << "iterations " << std::fixed << iteration
                  << "  dna " << std::fixed << dna.length()
                  << "  rna " << std::fixed << rna
                  << "  cost " << std::fixed << cost << '\n';
        exit(EXIT_SUCCESS);
    }

    void matchreplace(Pattern const& pat, Template const& t)
    {
        Number i = 0;
        Environment e;
        std::deque<Number> c;
        size_t start, count;

        for (auto const& p : pat) {
            switch (p.type) {
            case PItem::Type::Base:
                if (dna.at(i) == p.base) {
                    cost += 1;
                    i += 1;
                }
                else {
#if defined(TRACE)
                    std::cerr << "failed match\n";
#endif
                    return;
                }
                break;

            case PItem::Type::Skip:
                i += p.skip;
                if (i > static_cast<Number>(dna.length())) {
#if defined(TRACE)
                    std::cerr << "failed match\n";
#endif
                    return;
                }
                break;

            case PItem::Type::Search: {
                auto it = dna.search(p.search.to_string(), i);
                if (it != Rope::npos) {
                    auto n = it - i + p.search.length();
                    cost += n - i;
                    i += n;
                }
                else {
                    // failed match
                    cost += dna.length() - i;
                }
                break;
            }

            case PItem::Type::Open:
                c.push_front(i);
                break;

            case PItem::Type::Close:
                start = static_cast<size_t>(c.front());
                count = i - start;
                e.emplace_back(Rope{ dna.substring(start, count) });
                c.pop_front();
                break;
            }
        }

#if defined(TRACE)
        std::cerr << "succesful match of length " << i << '\n';
#endif

        dna.erase(0, i);
        replace(t, e);
    }

    Number nat()
    {
        Number n = 0;

        if (dna.empty()) {
            finish();
        }

        if (dna.at(0) == 'P') {
            dna.erase(0, 1);
            return 0;
        }

        auto end = dna.find('P');
        auto pos = end - 1;
        while (pos != 0) {
            switch (dna.at(pos)) {
            case 'I':
            case 'F':
                n *= 2;
                break;

            case 'C':
                n = 2 * n + 1;
                break;
            }

            --pos;
        }

        switch (dna.at(pos)) {
        case 'I':
        case 'F':
            n *= 2;
            break;

        case 'C':
            n = 2 * n + 1;
            break;
        }

        dna.erase(0, end + 1);

        return n;
    }

    Pattern pattern()
    {
        Pattern p;
        int lvl = 0;

        while (true) {
            if (dna_starts_with("C")) {
                dna.erase(0, 1);
                cost += 1;
                p.emplace_back(PItem('I'));
            }
            else if (dna_starts_with("F")) {
                dna.erase(0, 1);
                cost += 1;
                p.emplace_back(PItem('C'));
            }
            else if (dna_starts_with("P")) {
                dna.erase(0, 1);
                cost += 1;
                p.emplace_back(PItem('F'));
            }
            else if (dna_starts_with("IC")) {
                dna.erase(0, 2);
                cost += 2;
                p.emplace_back(PItem('P'));
            }
            else if (dna_starts_with("IP")) {
                dna.erase(0, 2);
                cost += 2;
                auto n = nat();
                p.emplace_back(PItem(n));
            }
            else if (dna_starts_with("IF")) {
                dna.erase(0, 3); // three bases consumed!
                cost += 3;
                auto s = consts();
                p.emplace_back(PItem(s));
            }
            else if (dna_starts_with("IIP")) {
                dna.erase(0, 3);
                cost += 3;
                lvl += 1;
                p.emplace_back(PItem(true)); // open group
            }
            else if (dna_starts_with("IIC") || dna_starts_with("IIF")) {
                dna.erase(0, 3);
                cost += 3;
                if (lvl == 0) {
                    return p;
                }
                lvl -= 1;
                p.emplace_back(PItem(false)); // close group
            }
            else if (dna_starts_with("III")) {
                std::string out{ dna.substring(3, 7) };
                dna.erase(0, 10);
                cost += 10;
                ++rna;
                std::cout << out << '\n';
            }
            else {
                // std::cerr << "# !!! pattern() Unrecognized DNA sequence!\nBail out!\n";
                finish();
            }
        }
    }

    DNA protect(Number l, DNA const& d)
    {
        if (l < 1) {
            return d;
        }

        DNA ret{ protect(l - 1, quote(d)) };

        if (l == 1) {
            cost += ret.length();
        }

        return ret;
    }

    DNA quote(DNA const& d) const
    {
        DNA result;

        auto size = d.length();

        for (size_t base = 0; base < size; ++base) {
            switch (d.at(base)) {
            case 'I':
                result.append("C");
                break;

            case 'C':
                result.append("F");
                break;

            case 'F':
                result.append("P");
                break;

            case 'P':
                result.append("IC");
                break;
            }
        }

        return result;
    }

    void read(std::string const& dna_file)
    {
        std::string data;
        std::ifstream inf;
        inf.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        inf.open(dna_file);
        inf >> data;
        inf.close();

        // remove any whitespace that might be in the input file
        data.erase(std::remove_if(std::begin(data), std::end(data),
                       [](char base) { return std::isspace(base); }),
            std::end(data));

        dna.append(data);
        if (!dna.is_balanced()) {
            dna.balance();
        }
    }

    void replace(Template const& tpl, Environment const& e)
    {
        DNA p, r, n;

#if defined(TRACE)
        std::cerr << to_string(e) << '\n';
#endif

        for (const auto& t : tpl) {
            switch (t.type) {
            case TItem::Type::Base:
                r.append(std::string{ t.base });
                break;

            case TItem::Type::Protection:
                if (t.prot.first != 0 || t.prot.second < e.size()) {
                    p = protect(t.prot.first, e[t.prot.second]);
                    r.append(p);
                }
                break;

            case TItem::Type::Reference:
                n = asnat(static_cast<Number>(e[t.ref].length()));
                r.append(n);
                break;
            }
        }

        dna.prepend(r);
    }

    Template templates()
    {
        Template t;

        while (true) {
            if (dna_starts_with("C")) {
                dna.erase(0, 1);
                cost += 1;
                t.emplace_back(TItem('I'));
            }
            else if (dna_starts_with("F")) {
                dna.erase(0, 1);
                cost += 1;
                t.emplace_back(TItem('C'));
            }
            else if (dna_starts_with("P")) {
                dna.erase(0, 1);
                cost += 1;
                t.emplace_back(TItem('F'));
            }
            else if (dna_starts_with("IC")) {
                dna.erase(0, 2);
                cost += 2;
                t.emplace_back(TItem('P'));
            }
            else if (dna_starts_with("IF") || dna_starts_with("IP")) {
                dna.erase(0, 2);
                cost += 2;
                auto l = nat();
                auto n = nat();
                t.emplace_back(TItem(l, n));
            }
            else if (dna_starts_with("IIC") || dna_starts_with("IIF")) {
                dna.erase(0, 3);
                cost += 3;
                return t;
            }
            else if (dna_starts_with("IIP")) {
                dna.erase(0, 3);
                cost += 3;
                auto n = nat();
                t.emplace_back(n);
            }
            else if (dna_starts_with("III")) {
                auto slice = dna.substring(3, 7);
                dna.erase(0, 10);
                cost += 11;
                std::cout << slice << '\n';
                ++rna;
            }
            else {
                // std::cerr << "# !!! tempates() Unrecognized DNA sequence!\nBail out!\n";
                finish();
            }
        }
    }

    static std::string to_string(DNA const& dna)
    {
        std::stringstream str;
        str << dna.substring(0, std::min(10ul, dna.length()));
        if (dna.length() >= 10) {
            str << "...";
        }
        str << " (" << dna.length() << " bases)";
        return str.str();
    }

    static std::string to_string(Environment const& env)
    {
        std::stringstream str;
        if (env.empty()) {
            str << "e is empty";
        }
        else {
            size_t i = 0;
            for (auto const& e : env) {
                if (i > 0) {
                    str << '\n';
                }

                str << "e[" << i++ << "] = ";

                str << e.substring(0, std::min(10ul, e.length()));
                if (e.length() >= 10) {
                    str << "...";
                }
                str << " (" << e.length() << " bases)";
            }
        }

        return str.str();
    }

    static std::string to_string(Pattern const& pat)
    {
        std::stringstream str;

        for (auto const& p : pat) {
            switch (p.type) {
            case PItem::Type::Base:
                str << p.base;
                break;

            case PItem::Type::Skip:
                str << '!' << p.skip;
                break;

            case PItem::Type::Search:
                str << '?' << p.search;
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

    static std::string to_string(Template const& templ)
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
};
