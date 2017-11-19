#define CATCH_CONFIG_MAIN
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

TEST_CASE("ColumnStore/Naive", "[unit]")
{
    SECTION("ColumnStore creation"){
        ColumnStore store = ColumnStore::Create_Naive(relation);

        REQUIRE(store.size() == 0);
        REQUIRE(store.size_in_bytes() == 0);
    }
}


