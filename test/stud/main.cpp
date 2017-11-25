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
}


