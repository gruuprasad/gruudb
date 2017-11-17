#include "catch.hpp"
#include "dbms/Schema.hpp"
#include "impl/RowStore.hpp"
#include <cstdint>
#include <string>
#include <iostream>

using namespace dbms;

namespace {

Relation relation("relation", {
        Attribute::Int1("int1"),                // 1 bytes + 3 padding
        Attribute::Float("float"),              // 4 bytes
        Attribute::Int8("int8"),                // 8 bytes
        Attribute::Char("Char(3)", 3),          // 3 bytes + 5 padding
        Attribute::Double("double"),            // 8 bytes
        Attribute::Varchar("Varchar(42)", 42),  // 8 bytes
        });                                     // TOTAL: 40 bytes


Relation lineitem("lineitem", {
            Attribute::Int1("returnflag"),            // 1 bytes + 7 padding
            Attribute::Int8("extendedprice"),         // 8 bytes 
            Attribute::Int1("linestatus"),            // 1 bytes + 7 padding
            Attribute::Int8("tax"),                   // 8 bytes 
            Attribute::Int4("orderkey"),              // 4 bytes
            Attribute::Int8("discount"),              // 8 bytes
            Attribute::Char("shipinstruct", 26),      // 26 bytes + 2 padding
            Attribute::Int4("partkey"),               // 4 bytes
            Attribute::Int4("suppkey"),               // 4 bytes
            Attribute::Int4("commitdate"),            // 4 bytes
            Attribute::Int4("receiptdate"),           // 4 bytes
            Attribute::Int4("shipdate"),              // 4 bytes
            Attribute::Int4("linenumber"),            // 4 bytes
            Attribute::Char("shipmode", 11),          // 11 bytes
            Attribute::Char("comment", 45),           // 45 bytes
            Attribute::Int8("quantity"),              // 8 bytes
            });
}

TEST_CASE("RowStore/Naive", "[unit]")
{
    RowStore store = RowStore::Create_Naive(lineitem);

    REQUIRE(store.num_attributes() == 16);
    REQUIRE(store.row_size() == 168);
    REQUIRE(store.size() == 0);
    REQUIRE(store.size_in_bytes() == 0);

    SECTION("reserving memory increases capacity but not change size") {
        store.reserve(10);
        REQUIRE(store.size() == 0);
        REQUIRE(store.capacity() >= 10);
    }
    
    SECTION("reserving smaller does not change size or capacity") {
        store.reserve(25);
        REQUIRE(store.size() == 0);
        REQUIRE(store.capacity() >= 25);
        store.reserve(10);
        REQUIRE(store.size() == 0);
        REQUIRE(store.capacity() >= 25);
    }
}
