#include "catch.hpp"
#include "dbms/Schema.hpp"
#include "impl/ColumnStore.hpp"
#include "impl/Compression.hpp"
#include <cstdint>
#include <string>


using namespace dbms;


TEST_CASE("RLE of int", "[unit][milestone2]")
{
    Column<RLE<int>> rle_col;

    REQUIRE(rle_col.size() == 0);
    REQUIRE(rle_col.size() == 0);
    REQUIRE(rle_col.num_runs() == 0);

    /* Insert 9 values in 4 runs. */
    rle_col.push_back(42);
    rle_col.push_back(42);
    rle_col.push_back(42);
    rle_col.push_back(42);
    rle_col.push_back( 7);
    rle_col.push_back( 7);
    rle_col.push_back(13);
    rle_col.push_back(42);
    rle_col.push_back(42);

    REQUIRE(rle_col.size() == 9);
    CHECK(rle_col.capacity() >= 9);
    CHECK(rle_col.num_runs() == 4);
    CHECK(rle_col.size_in_bytes() == 4 * sizeof(RLE<int>));
    CHECK(rle_col.capacity_in_bytes() >= 4 * sizeof(RLE<int>));

    {
        auto it = rle_col.begin();
        CHECK(*it++ == 42);
        CHECK(*it++ == 42);
        CHECK(*it++ == 42);
        CHECK(*it++ == 42);
        CHECK(*it++ ==  7);
        CHECK(*it++ ==  7);
        CHECK(*it++ == 13);
        CHECK(*it++ == 42);
        CHECK(*it++ == 42);
        REQUIRE(it == rle_col.end());

        CHECK(*--it == 42);
        CHECK(*--it == 42);
        CHECK(*--it == 13);
        CHECK(*--it ==  7);
        CHECK(*--it ==  7);
        CHECK(*--it == 42);
        CHECK(*--it == 42);
        CHECK(*--it == 42);
        CHECK(*--it == 42);
        CHECK(it == rle_col.begin());
    }

    {
        auto run_it = rle_col.runs_begin();
        CHECK(run_it->value == 42);
        CHECK(run_it->count == 4);
        ++run_it;
        CHECK(run_it->value == 7);
        CHECK(run_it->count == 2);
        ++run_it;
        CHECK(run_it->value == 13);
        CHECK(run_it->count == 1);
        ++run_it;
        CHECK(run_it->value == 42);
        CHECK(run_it->count == 2);
        ++run_it;
        CHECK(run_it == rle_col.runs_end());
    }
}

TEST_CASE("Dictionary", "[unit][milestone2]")
{
    const char *str0 = "Hello, World";
    const char *str1 = "This is a test";
    const char *str2 = "This string is new";

    Dictionary<std::string> string_dict;
    REQUIRE(string_dict.size() == 0);

    REQUIRE(string_dict(str0) == 0);
    REQUIRE(string_dict(str1) == 1);
    REQUIRE(string_dict(str0) == 0);
    REQUIRE(string_dict(str0) == 0);
    REQUIRE(string_dict(str1) == 1);
    REQUIRE(string_dict(str2) == 2);
    REQUIRE(string_dict(str1) == 1);
    REQUIRE(string_dict(str0) == 0);

    CHECK(string_dict.size() == 3);

    CHECK(string_dict[0] == str0);
    CHECK(string_dict[1] == str1);
    CHECK(string_dict[2] == str2);
}

TEST_CASE("Generic Dictionary Compression", "[unit][milestone2]")
{
    const char *str0 = "Hello, World";
    const char *str1 = "This is a test";
    const char *str2 = "This string is new";

    Column<Dictionary<Char<42>>> col;
    REQUIRE(col.size() == 0);
    REQUIRE(col.get_dictionary().size() == 0);

    col.push_back(str0);
    col.push_back(str1);
    col.push_back(str0);
    col.push_back(str0);
    col.push_back(str1);
    col.push_back(str2);
    col.push_back(str1);

    REQUIRE(col.size() == 7);
    REQUIRE(col.capacity() >= 7);
    REQUIRE(col.get_dictionary().size() == 3);
    CHECK(col.size_in_bytes() == sizeof(decltype(col)::dictionary_type::index_type) * col.size());

    auto &dict = col.get_dictionary();
    auto it = col.begin();
    CHECK(std::string(dict[*it++].data) == str0);
    CHECK(std::string(dict[*it++].data) == str1);
    CHECK(std::string(dict[*it++].data) == str0);
    CHECK(std::string(dict[*it++].data) == str0);
    CHECK(std::string(dict[*it++].data) == str1);
    CHECK(std::string(dict[*it++].data) == str2);
    CHECK(std::string(dict[*it++].data) == str1);
}

TEST_CASE("RLE on Dictionary Compression", "[unit][milestone2]")
{
    const char *str0 = "Hello, World";
    const char *str1 = "This is a test";
    const char *str2 = "This string is new";

    Column<RLE<Dictionary<Char<42>>>> col;
    REQUIRE(col.size() == 0);
    REQUIRE(col.get_dictionary().size() == 0);

    col.push_back(str0);
    col.push_back(str0);
    col.push_back(str0);
    col.push_back(str1);
    col.push_back(str1);
    col.push_back(str2);
    col.push_back(str1);

    REQUIRE(col.size() == 7);
    REQUIRE(col.num_runs() == 4);
    REQUIRE(col.capacity() >= 7);
    REQUIRE(col.get_dictionary().size() == 3);
    CHECK(col.size_in_bytes() == sizeof(decltype(col)::rle_type) * col.num_runs() + col.get_dictionary().size() * sizeof(Char<42>));

    auto &dict = col.get_dictionary();

    {
        auto it = col.begin();
        CHECK(std::string(dict[*it++].data) == str0);
        CHECK(std::string(dict[*it++].data) == str0);
        CHECK(std::string(dict[*it++].data) == str0);
        CHECK(std::string(dict[*it++].data) == str1);
        CHECK(std::string(dict[*it++].data) == str1);
        CHECK(std::string(dict[*it++].data) == str2);
        CHECK(std::string(dict[*it++].data) == str1);
    }

    {
        auto run_it = col.runs_begin();
        CHECK(std::string(dict[run_it->value].data) == str0);
        CHECK(run_it->count == 3);
        ++run_it;
        CHECK(std::string(dict[run_it->value].data) == str1);
        CHECK(run_it->count == 2);
        ++run_it;
        CHECK(std::string(dict[run_it->value].data) == str2);
        CHECK(run_it->count == 1);
        ++run_it;
        CHECK(std::string(dict[run_it->value].data) == str1);
        CHECK(run_it->count == 1);
    }
}
