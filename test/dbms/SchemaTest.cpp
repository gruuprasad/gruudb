#include "catch.hpp"
#include "dbms/Schema.hpp"


using namespace dbms;


TEST_CASE("Attribute/Int4", "[unit][core]")
{
    Attribute i4 = Attribute::Int4("myname");
    CHECK(i4.name == "myname");
    CHECK(i4.size == 4);
    CHECK(i4.type == Attribute::TY_Int);
}

TEST_CASE("Attribute/Varchar(42)", "[unit][core]")
{
    Attribute varchar = Attribute::Varchar("text", 42);
    CHECK(varchar.name == "text");
    CHECK(varchar.size == 42);
    CHECK(varchar.type == Attribute::TY_Varchar);
}

TEST_CASE("Relation", "[unit][core]")
{
    Relation test("test", {
            /* 00 */ Attribute::Int4("A"),
            /* 01 */ Attribute::Char("B", 6),
            /* 02 */ Attribute::Int1("C"),
            /* 03 */ Attribute::Varchar("D", 42),
            /* 04 */ Attribute::Float("E"),
            });

    CHECK(test.name == "test");
    REQUIRE(test.size() == 5);

    {
        auto &A = test[0];
        CHECK(A.name == "A");
        CHECK(&test["A"] == &A);
        CHECK(&test["a"] == &A);
        CHECK(A.offset() == 0);
    }

    {
        auto &E = test["e"];
        CHECK(E.name == "E");
        CHECK(&test[4] == &E);
        CHECK(E.offset() == 4);
    }

    {
        std::size_t i = 0;
        for (auto &attr : test) {
            CHECK(&attr == &test[attr.offset()]);
            CHECK(&attr == &test[attr.name]);
            CHECK(attr.offset() == i);
            ++i;
        }
    }
}
