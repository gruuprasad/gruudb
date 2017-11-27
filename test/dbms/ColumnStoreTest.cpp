#include "catch.hpp"
#include "dbms/Schema.hpp"
#include "impl/ColumnStore.hpp"
#include <cstdint>
#include <string>


using namespace dbms;


namespace {

Relation relation("relation", {
        Attribute::Int1("int1"),
        Attribute::Float("float"),
        Attribute::Int8("int8"),
        Attribute::Char("Char(3)", 3),
        Attribute::Double("double"),
        Attribute::Varchar("Varchar(42)", 42),
        });

}

TEST_CASE("ColumnStore/Naive", "[unit][milestone1]")
{
    ColumnStore store = ColumnStore::Create_Naive(relation);

    REQUIRE(store.size() == 0);
    REQUIRE(store.size_in_bytes() == 0);

    SECTION("access to columns via attribute offset") {
        auto &int1 = store.get_column<char>(0);
        CHECK(int1.size() == 0);
        CHECK(int1.size_in_bytes() == 0);
        CHECK(int1.elem_size() == 1);

        auto &char3 = store.get_column<Char<3>>(3);
        CHECK(char3.size() == 0);
        CHECK(char3.size_in_bytes() == 0);
        CHECK(char3.elem_size() == 3);
    }

    SECTION("add elements to columns via push_back and access through iterator") {
        auto &col_float = store.get_column<float>(1);
        auto &col_varchar = store.get_column<Varchar>(5);

        for (unsigned i = 0; i != 5; ++i) {
            col_float.push_back(3.14f * i);
            col_varchar.push_back(std::to_string(i).c_str());
        }

        REQUIRE(col_float.capacity() >= 5);
        REQUIRE(col_float.capacity_in_bytes() >= 5 * sizeof(float));
        REQUIRE(col_float.size() == 5);
        CHECK(col_float.size_in_bytes() == 5 * sizeof(float));

        REQUIRE(col_varchar.capacity() >= 5);
        REQUIRE(col_varchar.capacity_in_bytes() >= 5 * sizeof(const char*) + 2 * 5);
        REQUIRE(col_varchar.size() == 5);
        CHECK(col_varchar.size_in_bytes() == 5 * sizeof(const char*) + 2 * 5);

        auto float_it = col_float.begin();
        auto varchar_it = col_varchar.begin();
        for (unsigned i = 0; i != 5; ++i, ++float_it, ++varchar_it) {
            CHECK(3.14f * i == *float_it);
            CHECK(std::to_string(i) == std::string(*varchar_it));
        }
    }

    SECTION("adding many elements to a column grows its capacity") {
        auto &col_int8 = store.get_column<int64_t>(2);

        REQUIRE(col_int8.size() == 0);
        const auto old_cap = col_int8.capacity();

        for (auto i = 2 * old_cap; i; --i)
            col_int8.push_back(int64_t(i));

        REQUIRE(col_int8.capacity() >= 2 * old_cap);
        CHECK(col_int8.capacity_in_bytes() >= 2 * old_cap * 8);
        REQUIRE(col_int8.size() == 2 * old_cap);
    }
}
