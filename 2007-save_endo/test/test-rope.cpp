#include "rope.hpp"
#include <iostream>
#include <sstream>
#include <string>

size_t test_num = 0;

void test_constructors()
{
    bool ok;

    // default construction
    Rope r1{};
    ok = std::string{ "" } == r1.to_string();
    std::cout << (ok ? "" : "not ") << "ok " << ++test_num << " - default constructor\n";

    // construct from std::string
    const std::string string_1{ "This is a test." };
    Rope r2{ string_1 };
    ok = string_1 == r2.to_string();
    std::cout << (ok ? "" : "not ") << "ok " << ++test_num << " - construct from std::string\n";

    // copy constructor
    Rope r3{ r2 }; // NOLINT
    ok = r3.to_string() == r2.to_string();
    std::cout << (ok ? "" : "not ") << "ok " << ++test_num << " - copy constructor\n";
}

void test_assignment()
{
    const std::string paragraph_1{
        "Lorem ipsum dolor sit amet, consectetur adipiscing elit. "
        "Maecenas sapien diam, maximus a mauris sed, posuere tincidunt tellus. "
        "Morbi sapien enim, vehicula sed imperdiet vel, pharetra vel lorem. "
        "Nullam pharetra justo ac elit varius, ut accumsan nisl eleifend.  "
        "Mauris in condimentum augue. In consequat justo nunc, sit amet "
        "efficitur orci scelerisque at. Suspendisse ac ullamcorper urna, "
        "eget tincidunt risus. Suspendisse cursus nisl et volutpat ultrices. "
        "Integer posuere, diam vel tempus egestas, nisl leo tincidunt metus, "
        "nec semper risus nisl sit amet tortor. Morbi blandit sem sed nisi "
        "facilisis condimentum. Cras lacinia aliquet erat, nec finibus magna.  "
        "Curabitur efficitur ante vitae efficitur vestibulum. "
        "Nam a accumsan urna, vitae consectetur lorem. Proin rutrum ultrices "
        "sapien ac tincidunt. Phasellus semper vel leo quis semper."
    };

    bool ok;
    Rope r;

    const std::string string_1{ "This is a test." };
    Rope rTest{ string_1 };
    r = rTest;
    ok = rTest.to_string() == r.to_string();
    std::cout << (ok ? "" : "not ") << "ok " << ++test_num << " - assignment\n";

    rTest = rTest;
    ok = rTest.to_string() == rTest.to_string();
    std::cout << (ok ? "" : "not ") << "ok " << ++test_num << " - self assignment\n";

    // test multiple assignment
    Rope rParagraph{ paragraph_1 };
    r = rTest = rParagraph;
    ok = paragraph_1 == rTest.to_string();
    std::cout << (ok ? "" : "not ") << "ok " << ++test_num << " - multiple assignment part 1\n";
    ok = paragraph_1 == r.to_string();
    std::cout << (ok ? "" : "not ") << "ok " << ++test_num << " - multiple assignment part 2\n";
}

void test_strings_equal()
{
    bool ok;
    const std::string string_1{ "This is a test." };

    Rope rEmpty{};
    Rope r1{ string_1 };
    Rope r1_Mutated{ string_1 };

    ok = rEmpty != r1;
    std::cout << (ok ? "" : "not ") << "ok " << ++test_num << " - not equal\n";

    ok = r1 == r1_Mutated;
    std::cout << (ok ? "" : "not ") << "ok " << ++test_num << " - equal\n";

    r1_Mutated.erase(4, 1);
    r1_Mutated.erase(6, 1);
    r1_Mutated.erase(7, 1);
    Rope tmpRope{ "Thisisatest." };
    ok = r1_Mutated == tmpRope;
    std::cout << (ok ? "" : "not ") << "ok " << ++test_num << " - equal (mutated)\n";

    r1_Mutated.insert(4, " ");
    r1_Mutated.insert(7, " ");
    r1_Mutated.insert(9, " ");
    ok = r1_Mutated == r1;
    std::cout << (ok ? "" : "not ") << "ok " << ++test_num << " - equal (unmutated)\n";
}

void test_at()
{
    const std::string string_1{ "This is a test." };
    const std::string string_2{ "Here is a much longer string for testing!" };
    bool ok;

    Rope r{ string_1 };
    ok = r.at(0) == 'T';
    std::cout << (ok ? "" : "not ") << "ok " << ++test_num << " - at(0)\n";

    ok = r.at(8) == 'a';
    std::cout << (ok ? "" : "not ") << "ok " << ++test_num << " - at(8)\n";

    r = Rope(string_2);
    ok = 'm' == r.at(10);
    std::cout << (ok ? "" : "not ") << "ok " << ++test_num << " - string_2 at(10)\n";

    bool caught = false;
    try {
        r.at(102);
    }
    catch (std::out_of_range const&) {
        caught = true;
    }
    std::cout << (caught ? "" : "not ") << "ok " << ++test_num << " - exception for at() past end\n";
}

void test_length()
{
    const std::string string_1{ "This is a test." };
    const std::string string_2{ "Here is a much longer string for testing!" };
    bool ok;

    Rope rope_0{};
    ok = 0 == rope_0.length();
    std::cout << (ok ? "" : "not ") << "ok " << ++test_num << " - rope_0 length is 0\n";

    Rope rope_15{ string_1 };
    ok = 15 == rope_15.length();
    std::cout << (ok ? "" : "not ") << "ok " << ++test_num << " - rope_15 length is 15\n";

    Rope rope_41{ string_2 };
    ok = 41 == rope_41.length();
    std::cout << (ok ? "" : "not ") << "ok " << ++test_num << " - rope_41 length is 41\n";
}

void test_insert()
{
    const std::string string_1{ "This is a test." };
    bool ok, caught;

    Rope rEmpty{};
    Rope r1{ string_1 };

    // test out-of-range indices
    caught = false;
    try {
        rEmpty.insert(1, "text");
    }
    catch (std::out_of_range const&) {
        caught = true;
    }
    std::cout << (caught ? "" : "not ") << "ok " << ++test_num << " - exception for insert() past end of empty rope\n";

    caught = false;
    try {
        r1.insert(120, "");
    }
    catch (std::out_of_range const&) {
        caught = true;
    }
    std::cout << (caught ? "" : "not ") << "ok " << ++test_num << " - exception for insert() past end\n";

    rEmpty.insert(0, "");
    ok = std::string{ "" } == rEmpty.to_string();
    std::cout << (ok ? "" : "not ") << "ok " << ++test_num << " - insert empty string into empty rope\n";

    rEmpty.insert(0, string_1);
    ok = string_1 == rEmpty.to_string();
    std::cout << (ok ? "" : "not ") << "ok " << ++test_num << " - insert string into empty rope\n";

    r1.insert(0, "Hello ");
    ok = std::string{ "Hello This is a test." } == r1.to_string();
    std::cout << (ok ? "" : "not ") << "ok " << ++test_num << " - insert string into front of rope\n";

    r1.insert(r1.length(), " Bye!");
    ok = std::string{ "Hello This is a test. Bye!" } == r1.to_string();
    std::cout << (ok ? "" : "not ") << "ok " << ++test_num << " - insert string onto end of rope\n";

    r1.insert(r1.length() - 1, " (more text)");
    ok = std::string{ "Hello This is a test. Bye (more text)!" } == r1.to_string();
    std::cout << (ok ? "" : "not ") << "ok " << ++test_num << " - insert string into rope\n";

    rEmpty = Rope{};
    r1 = Rope{ string_1 };

    rEmpty.insert(0, rEmpty);
    ok = std::string{ "" } == rEmpty.to_string();
    std::cout << (ok ? "" : "not ") << "ok " << ++test_num << " - self insert empty rope\n";

    rEmpty.insert(0, r1);
    ok = string_1 == rEmpty.to_string();
    std::cout << (ok ? "" : "not ") << "ok " << ++test_num << " - insert non-empty rope\n";

    Rope rHello{ "Hello " };
    r1.insert(0, rHello);
    ok = std::string{ "Hello This is a test." } == r1.to_string();
    std::cout << (ok ? "" : "not ") << "ok " << ++test_num << " - insert non-empty rope into non-empty rope\n";
}

void test_append()
{
    const std::string string_1{ "This is a test." };
    const std::string string_2{ "Here is a much longer string for testing!" };
    bool ok;

    // test concatenation of ropes representing non-empty strings
    Rope r1{ string_1 };
    Rope r2{ string_2 };
    r1.append(r2);
    ok = (string_1 + string_2) == r1.to_string();
    std::cout << (ok ? "" : "not ") << "ok " << ++test_num << " - concatenate two non-empty ropes\n";

    // test concatenation of two ropes both representing the empty string
    Rope rEmpty{};
    rEmpty.append(rEmpty);
    ok = std::string{ "" } == rEmpty.to_string();
    std::cout << (ok ? "" : "not ") << "ok " << ++test_num << " - concatenate two empty ropes\n";

    // test concatenation of 'empty' rope with a rope representing a non-empty string
    r2.append(rEmpty);
    ok = string_2 == r2.to_string();
    std::cout << (ok ? "" : "not ") << "ok " << ++test_num << " - concatenate non-empty rope to empty rope\n";
}

void test_at_plus_length_plus_append()
{
    const std::string string_1{ "This is a test." };
    const std::string string_2{ "Here is a much longer string for testing!" };
    bool ok;

    Rope rEmpty{};
    Rope r1{ string_1 };
    Rope r2{ string_2 };

    // concatenate empty string and non-empty string
    rEmpty.append(r1);
    ok = 'T' == rEmpty.at(0);
    std::cout << (ok ? "" : "not ") << "ok " << ++test_num << " - append at 0\n";
    ok = 'a' == rEmpty.at(8);
    std::cout << (ok ? "" : "not ") << "ok " << ++test_num << " - append at 8\n";
    ok = 15 == rEmpty.length();
    std::cout << (ok ? "" : "not ") << "ok " << ++test_num << " - length after append\n";

    rEmpty.append(r2);
    ok = 'T' == rEmpty.at(0);
    std::cout << (ok ? "" : "not ") << "ok " << ++test_num << " - append2 at 0\n";
    ok = '!' == rEmpty.at(55);
    std::cout << (ok ? "" : "not ") << "ok " << ++test_num << " - append2 at 55\n";
    ok = 56 == rEmpty.length();
    std::cout << (ok ? "" : "not ") << "ok " << ++test_num << " - length after append2\n";
}

void test_substring()
{
    const std::string paragraph_1{
        "Lorem ipsum dolor sit amet, consectetur adipiscing elit. "
        "Maecenas sapien diam, maximus a mauris sed, posuere tincidunt tellus. "
        "Morbi sapien enim, vehicula sed imperdiet vel, pharetra vel lorem. "
        "Nullam pharetra justo ac elit varius, ut accumsan nisl eleifend.  "
        "Mauris in condimentum augue. In consequat justo nunc, sit amet "
        "efficitur orci scelerisque at. Suspendisse ac ullamcorper urna, "
        "eget tincidunt risus. Suspendisse cursus nisl et volutpat ultrices. "
        "Integer posuere, diam vel tempus egestas, nisl leo tincidunt metus, "
        "nec semper risus nisl sit amet tortor. Morbi blandit sem sed nisi "
        "facilisis condimentum. Cras lacinia aliquet erat, nec finibus magna.  "
        "Curabitur efficitur ante vitae efficitur vestibulum. "
        "Nam a accumsan urna, vitae consectetur lorem. Proin rutrum ultrices "
        "sapien ac tincidunt. Phasellus semper vel leo quis semper."
    };

    const std::string string_1{ "This is a test." };
    bool ok, caught;

    Rope rEmpty;
    Rope r1{ string_1 };
    Rope rParagraph{ paragraph_1 };

    // test out-of-range substring indices
    caught = false;
    try {
        rEmpty.substring(0, 1);
    }
    catch (std::invalid_argument const&) {
        caught = true;
    }
    std::cout << (caught ? "" : "not ") << "ok " << ++test_num << " - exception for substring(0, 1) on empty rope\n";

    caught = false;
    try {
        rEmpty.substring(1, 0);
    }
    catch (std::invalid_argument const&) {
        caught = true;
    }
    std::cout << (caught ? "" : "not ") << "ok " << ++test_num << " - exception for substring(1, 0) on empty rope\n";

    caught = false;
    try {
        r1.substring(1, 120);
    }
    catch (std::invalid_argument const&) {
        caught = true;
    }
    std::cout << (caught ? "" : "not ") << "ok " << ++test_num << " - exception for substring out of range on rope\n";

    // test empty string return conditions
    ok = std::string{ "" } == rEmpty.substring(0, 0);
    std::cout << (ok ? "" : "not ") << "ok " << ++test_num << " - empty substring on empty rope\n";

    ok = std::string{ "" } == r1.substring(4, 0);
    std::cout << (ok ? "" : "not ") << "ok " << ++test_num << " - empty substring on rope\n";

    // test non-zero substring length for valid substrings
    ok = std::string{ "This" } == r1.substring(0, 4);
    std::cout << (ok ? "" : "not ") << "ok " << ++test_num << " - substring at front of rope\n";

    ok = std::string{ "test." } == r1.substring(10, 5);
    std::cout << (ok ? "" : "not ") << "ok " << ++test_num << " - substring at end of rope\n";

    ok = std::string{ " elit. Maecenas sapien diam, maximus a mauris sed," } == rParagraph.substring(50, 50);
    std::cout << (ok ? "" : "not ") << "ok " << ++test_num << " - substring from long rope\n";
}

void test_delete()
{
    const std::string string_1{ "This is a test." };
    bool ok, caught;

    Rope rEmpty;
    Rope r1{ string_1 };
    Rope r2{ "Hello This is a test. Bye (more text)!" };

    // test out-of-range indices
    caught = false;
    try {
        rEmpty.erase(1, 0);
    }
    catch (std::out_of_range const&) {
        caught = true;
    }
    std::cout << (caught ? "" : "not ") << "ok " << ++test_num << " - exception for erase(0, 1) on empty rope\n";

    caught = false;
    try {
        r1.erase(120, 1);
    }
    catch (std::out_of_range const&) {
        caught = true;
    }
    std::cout << (caught ? "" : "not ") << "ok " << ++test_num << " - exception for out-of-range erase()\n";

    // test invalid length parameters
    caught = false;
    try {
        rEmpty.erase(0, 1);
    }
    catch (std::out_of_range const&) {
        caught = true;
    }
    std::cout << (!caught ? "" : "not ") << "ok " << ++test_num << " - no exception for out-of-range erase() on empty rope\n";

    caught = false;
    try {
        r1.erase(9, 14);
    }
    catch (std::out_of_range const&) {
        caught = true;
    }
    std::cout << (!caught ? "" : "not ") << "ok " << ++test_num << " - no exception for out-of-range erase() on rope\n";

    rEmpty.erase(0, 0);
    ok = std::string{ "" } == rEmpty.to_string();
    std::cout << (ok ? "" : "not ") << "ok " << ++test_num << " - delete nothing from empty rope\n";

    r1 = string_1;
    r1.erase(4, 0);
    ok = string_1 == r1.to_string();
    std::cout << (ok ? "" : "not ") << "ok " << ++test_num << " - delete nothing from non-empty rope\n";

    auto r2_len = r2.length();
    r2.erase(r2_len - 13, 12);
    ok = std::string{ "Hello This is a test. Bye!" } == r2.to_string();
    std::cout << (ok ? "" : "not ") << "ok " << ++test_num << " - delete from middle of rope\n";

    r2_len = r2.length();
    r2.erase(r2_len - 5, 5);
    ok = std::string{ "Hello This is a test." } == r2.to_string();
    std::cout << (ok ? "" : "not ") << "ok " << ++test_num << " - delete from end of rope\n";

    r2.erase(0, 6);
    ok = string_1 == r2.to_string();
    std::cout << (ok ? "" : "not ") << "ok " << ++test_num << " - delete from beginning of string\n";

    r2.erase(0, r2.length());
    ok = std::string{ "" } == r2.to_string();
    std::cout << (ok ? "" : "not ") << "ok " << ++test_num << " - delete an entire string\n";
}

void test_balance()
{
    const std::string string_1{ "This is a test." };
    bool ok;

    Rope rF{ "f" };
    Rope rE{ "e" };
    Rope rD{ "d" };
    Rope rC{ "c" };
    Rope rB{ "b" };
    Rope rA{ "a" };

    ok = rF.is_balanced();
    std::cout << (ok ? "" : "not ") << "ok " << ++test_num << " - single character rope is balanced\n";

    Rope r1{ string_1 };
    ok = r1.is_balanced();
    std::cout << (ok ? "" : "not ") << "ok " << ++test_num << " - rope is balanced\n";

    rF.insert(0, rE);
    ok = !rF.is_balanced();
    std::cout << (ok ? "" : "not ") << "ok " << ++test_num << " - rope is not balanced (1)\n";

    rF.insert(0, rD);
    rF.insert(0, rC);
    rF.insert(0, rB);
    rF.insert(0, rA);
    ok = !rF.is_balanced();
    std::cout << (ok ? "" : "not ") << "ok " << ++test_num << " - rope is not balanced (2)\n";

    rF.balance();
    ok = rF.is_balanced();
    std::cout << (ok ? "" : "not ") << "ok " << ++test_num << " - rope is balanced after calling balance()\n";

    ok = std::string{ "abcdef" } == rF.to_string();
    std::cout << (ok ? "" : "not ") << "ok " << ++test_num << " - after calling balance() string still compares\n";
}

// explode function for use in testing
// usage: auto v = explode("hello world foo bar", ' ');
std::vector<Rope*> explode(std::string const& str, char delim)
{
    std::vector<Rope*> result;
    std::istringstream iss(str);
    for (std::string token; std::getline(iss, token, delim); /*NULL*/) {
        auto r = new Rope(token);
        result.push_back(r);
    }
    return result;
}

// recover memory allocated via the explode function above
void reap_exploded(std::vector<Rope*>& v)
{
    for (auto iter = std::begin(v); iter != std::end(v); ++iter) {
        delete *iter;
    }
    v.clear();
}

void test_build_and_balance()
{
    const std::string paragraph_1{
        "Lorem ipsum dolor sit amet, consectetur adipiscing elit. "
        "Maecenas sapien diam, maximus a mauris sed, posuere tincidunt tellus. "
        "Morbi sapien enim, vehicula sed imperdiet vel, pharetra vel lorem. "
        "Nullam pharetra justo ac elit varius, ut accumsan nisl eleifend.  "
        "Mauris in condimentum augue. In consequat justo nunc, sit amet "
        "efficitur orci scelerisque at. Suspendisse ac ullamcorper urna, "
        "eget tincidunt risus. Suspendisse cursus nisl et volutpat ultrices. "
        "Integer posuere, diam vel tempus egestas, nisl leo tincidunt metus, "
        "nec semper risus nisl sit amet tortor. Morbi blandit sem sed nisi "
        "facilisis condimentum. Cras lacinia aliquet erat, nec finibus magna.  "
        "Curabitur efficitur ante vitae efficitur vestibulum. "
        "Nam a accumsan urna, vitae consectetur lorem. Proin rutrum ultrices "
        "sapien ac tincidunt. Phasellus semper vel leo quis semper."
    };

    bool ok;
    std::vector<Rope*> exploded = explode(paragraph_1, ' ');

    Rope* rParagraph = exploded[0];
    size_t tmpLen;
    for (auto iter = ++exploded.begin(); iter != exploded.end(); iter++) {
        tmpLen = rParagraph->length();
        rParagraph->insert(tmpLen, " ");
        rParagraph->insert(tmpLen + 1, **iter);
    }

    ok = paragraph_1 == rParagraph->to_string(); // compare to original text
    std::cout << (ok ? "" : "not ") << "ok " << ++test_num << " - explode and build matches original text\n";

    ok = !rParagraph->is_balanced();
    std::cout << (ok ? "" : "not ") << "ok " << ++test_num << " - explode and build is not balanced\n";

    rParagraph->balance();
    ok = rParagraph->is_balanced();
    std::cout << (ok ? "" : "not ") << "ok " << ++test_num << " - after balance() is balanced\n";

    reap_exploded(exploded);
}

void test_search()
{
    const std::string string_2{ "Here is a much longer string for testing!" };
    bool ok;

    Rope rope{ string_2 };

    auto result = rope.search("much");
    ok = rope.search("much") != Rope::npos;
    std::cout << (ok ? "" : "not ") << "ok " << ++test_num << " - found string in rope\n";

    ok = result == 10;
    std::cout << (ok ? "" : "not ") << "ok " << ++test_num << " - found string at expected position\n";

    rope = std::string{ "one two three two one" };
    result = rope.search("two", 5);
    ok = result != Rope::npos;
    std::cout << (ok ? "" : "not ") << "ok " << ++test_num << " - search(\"two\", 5) found\n";

    ok = result == 14;
    std::cout << (ok ? "" : "not ") << "ok " << ++test_num << " - search(\"two\", 5) found at expected position\n";

    ok = rope.search("bogus") == Rope::npos;
    std::cout << (ok ? "" : "not ") << "ok " << ++test_num << " - did not find string in rope\n";
}

int main()
{
    test_num = 0;

    test_constructors();
    test_assignment();
    test_strings_equal();
    test_at();
    test_length();
    test_insert();
    test_append();
    test_at_plus_length_plus_append();
    test_substring();
    test_delete();
    test_balance();
    test_build_and_balance();
    test_search();

    std::cout << "not ok " << ++test_num << " - testing 'not ok'\n";

    std::cout << "1.." << test_num << '\n';

    return 0;
}
