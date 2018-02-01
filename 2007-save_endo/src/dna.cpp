#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <iterator>
#include <sstream>

#if defined(POOR_PROFILE)
#include <chrono>
#endif

#if defined(POOR_FUNC_COUNTER)
struct Prof_Function_Counters {
    unsigned asnat{ 0 };
    unsigned consts{ 0 };
    unsigned nat{ 0 };
    unsigned matchreplace{ 0 };
    unsigned pattern{ 0 };
    unsigned protect{ 0 };
    unsigned quote{ 0 };
    unsigned replace{ 0 };
    unsigned templates{ 0 };
};

static Prof_Function_Counters pfc;
#endif

#include "dna.hpp"

DNA dna; /* NOLINT */ // @TODO(bml) - remove warning via refactor

size_t n_rna, iteration;

[[maybe_unused]] std::string
to_string(const DNA& dna)
{
    std::stringstream str;
    auto it = std::begin(dna);
    for (size_t p = 0; p < 10 && it != std::end(dna); ++it, ++p) {
        if (it != std::end(dna)) {
            str << *it;
        }
    }
    if (it != std::end(dna)) {
        str << "...";
    }
    return str.str();
}

[[maybe_unused]] std::string
to_string(const Environment& env)
{
    std::stringstream str;
    if (env.empty()) {
        str << "e is empty";
    }
    else {
        size_t i = 0;
        for (const auto& e : env) {
            if (i > 0) {
                str << '\n';
            }

            str << "e[" << i++ << "] = ";

            auto it = std::begin(e);
            for (size_t j = 0; j < 10 && it != std::end(e); ++it, ++j) {
                if (it != std::end(e)) {
                    str << *it;
                }
            }
            if (it != std::end(e)) {
                str << "...";
            }
            str << " (" << e.size() << " bases)";
        }
    }

    return str.str();
}

[[maybe_unused]] std::string
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
            for (auto it = std::begin(p.search); it != std::end(p.search); ++it) {
                str << *it;
            }
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

[[maybe_unused]] std::string
to_string(const Template& templ)
{
    std::stringstream str;
    for (const auto& t : templ) {
        switch (t.type) {
        case TItem::Type::Base:
            str << t.base; /* NOLINT */
            break;

        case TItem::Type::Protection:
            str << '\\' << t.prot.second; /* NOLINT */
            break;

        case TItem::Type::Reference:
            str << '|' << t.ref << '|'; /* NOLINT */
            break;
        }
    }

    return str.str();
}

static inline bool
dna_starts_with(const std::string& value)
{
    if (dna.empty() || value.empty()) {
        return false;
    }

    auto it = std::begin(dna);
    for (auto base : value) {
        if (base != *it) {
            return false;
        }
        ++it;
    }

    return true;
}

DNA asnat(Number n)
{
#if defined(POOR_FUNC_COUNTER)
    pfc.asnat++;
#endif

    DNA result;

    while (n > 0) {
        if ((n & 1) != 0) {
            result.emplace_back('C');
        }
        else {
            result.emplace_back('I');
        }

        n /= 2;
    }

    result.emplace_back('P');

    return result;
}

DNA consts()
{
#if defined(POOR_FUNC_COUNTER)
    pfc.consts++;
#endif

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
    try {
        iteration = -1;
        n_rna = 0;

#if defined(POOR_PROFILE)
        auto prof_start = std::chrono::high_resolution_clock::now();
#endif

        while (true) {
            ++iteration;

#if defined(TRACE)
            std::cerr << "iteration " << iteration << '\n';
            std::cerr << "dna = " << to_string(dna) << " (" << dna.size() << " bases)\n";
#else
            if ((iteration % 1024u) == 0) {
#if defined(POOR_PROFILE)
                auto prof_end = std::chrono::high_resolution_clock::now();
                auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(prof_end - prof_start).count();
#endif

                // std::cerr << "iteration " << iteration << "  dna " << dna.size() << "  rna " << rna.size() << '\n';
                std::cerr
                    << "iteration " << iteration
                    << "  dna " << dna.size()
                    << "  rna " << n_rna
#if defined(POOR_PROFILE)
                    << "  elapsed " << elapsed << "s"
                    << "  iter/sec " << (static_cast<double>(iteration) / static_cast<double>(elapsed))
                    << "  sec/iter " << (static_cast<double>(elapsed) / static_cast<double>(iteration))
#endif
                    << '\n';

#if defined(POOR_FUNC_COUNTER)
                std::cerr
                    << "counts"
                    << " asnat " << pfc.asnat
                    << " consts " << pfc.consts
                    << " nat " << pfc.nat
                    << " matchreplace " << pfc.matchreplace
                    << " pattern " << pfc.pattern
                    << " protect " << pfc.protect
                    << " quote " << pfc.quote
                    << " replace " << pfc.replace
                    << " templates " << pfc.templates
                    << '\n';
#endif
            }
#endif

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
            // std::cerr << "len(rna) = " << rna.size() << "\n\n";
            std::cerr << "len(rna) = " << n_rna << "\n\n";
#endif
        }
    }
    catch (const std::exception& ex) {
        std::cerr
            << "exception " << ex.what()
            << "\niterations " << iteration
            << "\ndna = " << to_string(dna) << " (" << dna.size() << " bases)"
            << "\nlen(rna) = " << n_rna << "\n\n";
    }
}

[[noreturn]] void finish()
{
    std::cerr
        << "iterations " << iteration
        << "\ndna = " << to_string(dna) << " (" << dna.size() << " bases)"
        << "\nlen(rna) = " << n_rna << "\n\n";

    exit(EXIT_SUCCESS);
}

void matchreplace(const Pattern& pat, const Template& t)
{
#if defined(POOR_FUNC_COUNTER)
    pfc.matchreplace++;
#endif

    Number i = 0;
    Environment e;
    std::deque<Number> c;

    for (const auto& p : pat) {
        switch (p.type) {
        case PItem::Type::Base:
            if (dna[i] == p.base) {
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
            if (i > static_cast<Number>(dna.size())) {
#if defined(TRACE)
                std::cerr << "failed match\n";
#endif
                return;
            }
            break;

        case PItem::Type::Search: {
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
            e.emplace_back(DNA(std::begin(dna) + c.front(), std::begin(dna) + i));
            c.pop_front();
            break;
        }
    }

#if defined(TRACE)
    std::cerr << "succesful match of length " << i << '\n';
#endif

    dna.erase(std::begin(dna), std::begin(dna) + i);
    replace(t, e);
}

Number nat()
{
#if defined(POOR_FUNC_COUNTER)
    pfc.nat++;
#endif

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
#if defined(POOR_FUNC_COUNTER)
    pfc.pattern++;
#endif

    Pattern p;
    int lvl = 0;

    while (true) {
        if (dna_starts_with("C")) {
            dna.pop_front();
            p.emplace_back(PItem('I'));
        }
        else if (dna_starts_with("F")) {
            dna.pop_front();
            p.emplace_back(PItem('C'));
        }
        else if (dna_starts_with("P")) {
            dna.pop_front();
            p.emplace_back(PItem('F'));
        }
        else if (dna_starts_with("IC")) {
            dna.erase(std::begin(dna), std::begin(dna) + 2);
            p.emplace_back(PItem('P'));
        }
        else if (dna_starts_with("IP")) {
            dna.erase(std::begin(dna), std::begin(dna) + 2);
            auto n = nat();
            p.emplace_back(PItem(n));
        }
        else if (dna_starts_with("IF")) {
            dna.erase(std::begin(dna), std::begin(dna) + 3); // three bases consumed!
            auto s = consts();
            p.emplace_back(PItem(s));
        }
        else if (dna_starts_with("IIP")) {
            dna.erase(std::begin(dna), std::begin(dna) + 3);
            lvl += 1;
            p.emplace_back(PItem(true)); // open group
        }
        else if (dna_starts_with("IIC") || dna_starts_with("IIF")) {
            dna.erase(std::begin(dna), std::begin(dna) + 3);
            if (lvl == 0) {
                return p;
            }
            lvl -= 1;
            p.emplace_back(PItem(false)); // close group
        }
        else if (dna_starts_with("III")) {
            // rna.push_back(DNA(std::begin(dna) + 3, std::begin(dna) + 10));
            DNA out(std::begin(dna) + 3, std::begin(dna) + 10);
            dna.erase(std::begin(dna), std::begin(dna) + 10);
            for (auto b : out) {
                std::cout << b;
            }
            std::cout << std::endl;
            ++n_rna;
        }
        else {
            // std::cerr << "# !!! pattern() Unrecognized DNA sequence!\nBail out!\n";
            finish();
        }
    }
}

DNA protect(Number l, const DNA& d)
{
#if defined(POOR_FUNC_COUNTER)
    pfc.protect++;
#endif

    return l == 0 ? d : protect(l - 1, quote(d));
}

DNA quote(const DNA& d)
{
#if defined(POOR_FUNC_COUNTER)
    pfc.quote++;
#endif

    DNA result;

#if 0 // recursive version
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
#else
    for (auto base : d) {
        switch (base) {
        case 'I':
            result.push_back('C');
            break;

        case 'C':
            result.push_back('F');
            break;

        case 'F':
            result.push_back('P');
            break;

        case 'P':
            result.push_back('I');
            result.push_back('C');
            break;
        }
    }
#endif

    return result;
}

void replace(const Template& tpl, const Environment& e)
{
#if defined(POOR_FUNC_COUNTER)
    pfc.replace++;
#endif

    DNA p, r, n;

#if defined(TRACE)
    std::cerr << to_string(e) << '\n';
#endif

    for (const auto& t : tpl) {
        switch (t.type) {
        case TItem::Type::Base:
            r.push_back(t.base); /* NOLINT */
            break;

        case TItem::Type::Protection:
            p = protect(t.prot.first, e[t.prot.second]); /* NOLINT */
            r.insert(std::end(r), std::begin(p), std::end(p));
            break;

        case TItem::Type::Reference:
            n = asnat(static_cast<Number>(e[t.ref].size())); /* NOLINT */
            r.insert(std::end(r), std::begin(n), std::end(n));
            break;
        }
    }

    dna.insert(std::begin(dna), std::begin(r), std::end(r));
}

Template templates()
{
#if defined(POOR_FUNC_COUNTER)
    pfc.templates++;
#endif

    Template t;

    while (true) {
        if (dna_starts_with("C")) {
            dna.pop_front();
            t.emplace_back(TItem('I'));
        }
        else if (dna_starts_with("F")) {
            dna.pop_front();
            t.emplace_back(TItem('C'));
        }
        else if (dna_starts_with("P")) {
            dna.pop_front();
            t.emplace_back(TItem('F'));
        }
        else if (dna_starts_with("IC")) {
            dna.erase(std::begin(dna), std::begin(dna) + 2);
            t.emplace_back(TItem('P'));
        }
        else if (dna_starts_with("IF") || dna_starts_with("IP")) {
            dna.erase(std::begin(dna), std::begin(dna) + 2);
            auto l = nat();
            auto n = nat();
            t.emplace_back(TItem(l, n));
        }
        else if (dna_starts_with("IIC") || dna_starts_with("IIF")) {
            dna.erase(std::begin(dna), std::begin(dna) + 3);
            return t;
        }
        else if (dna_starts_with("IIP")) {
            dna.erase(std::begin(dna), std::begin(dna) + 3);
            auto n = nat();
            t.emplace_back(n);
        }
        else if (dna_starts_with("III")) {
            DNA slice(std::begin(dna) + 3, std::begin(dna) + 11);
            dna.erase(std::begin(dna), std::begin(dna) + 11);
            for (auto b : slice) {
                std::cout << b;
            }
            std::cout << std::endl;
            ++n_rna;
        }
        else {
            // std::cerr << "# !!! tempates() Unrecognized DNA sequence!\nBail out!\n";
            finish();
        }
    }
}
